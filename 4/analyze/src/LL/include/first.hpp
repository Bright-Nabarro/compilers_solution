#pragma once
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>

#include "grammar.hpp"

namespace analyze
{

class FirstSetException: public std::logic_error
{
public:
	FirstSetException(std::string_view sv):
		std::logic_error(sv.data())
	{};
};

/*
 * 要求用户保证外部的Grammar不可变
 * 如果文法中有`孤岛`产生式D，则 has_set(D)返回false
 * 这里空集的类型采取 {Symbol::other, "e"s}, 与Grammar表示的不同
 */
class FirstSet
{
public:
	using SymbolsSet = std::unordered_set<Symbol>;
	explicit FirstSet(const Grammar& grammar);
	virtual ~FirstSet() = default;
	[[nodiscard]]
	std::optional<SymbolsSet> find_first(const Symbol& symbol) const;
	/// 查找check是否存在于find的first集中
	[[nodiscard]]
	bool in_firstset(const Symbol& find, const Symbol& check) const;
private:
	/*
	 * construct 中产生的tl::expected以异常抛出
	 * 用户层错误实际应由Parser解析时, 在此时判断应为逻辑错误
	 */
	void construct();
	
	using RulesSet = std::unordered_set<std::vector<analyze::Symbol>,
		  analyze::SymbolListHash>;

	template<typename RetType>
	RetType check_expect(const tl::expected<RetType, std::string>& ret)
	{
		if (!ret)
			throw FirstSetException{ret.error()};
		return *ret;
	}

	/*
	 * 1) if X.type() == terminal，then FIRST(X) += X
	 * 2) if X.type() == no_terminal, then FIRST(X) += FIRST(Y_1), if Y_1.nullable() -> FIRST(X) += FIRST(Y_2) ...
	 * 3) if X.nullable(), then FIRST(X) += empty_symbol
	 */
	SymbolsSet find_first_recursive(const Symbol& symbol);

private:
	const Grammar& m_grammar;
	std::unordered_map<Symbol, SymbolsSet> m_first_set;
};

}	//namespace analyze
