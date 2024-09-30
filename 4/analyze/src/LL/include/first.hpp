#pragma once
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <queue>

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
	virtual ~FirstSet() = default;
	explicit FirstSet(const Grammar& grammar);
	///要求用户保证`has_set`为true
	[[nodiscard]]
	auto get_set(const Symbol& symbol) const
		-> const std::unordered_set<Symbol>;
	[[nodiscard]]
	bool has_set(const Symbol& symbol) const;
private:
	/*
	 * construct 中产生的tl::expected以异常抛出
	 * 用户层错误实际应由Parser解析时, 在此时判断应为逻辑错误
	 */
	void construct();
	
	using RulesSet = std::unordered_set<std::vector<analyze::Symbol>,
		  analyze::SymbolListHash>;
	///插入两个容器: queue和m_firstSet
	void unique_insert(std::queue<Symbol>& queue, Symbol left, RulesSet right);


	template<typename RetType>
	RetType check_expect(const tl::expected<RetType, std::string>& ret)
	{
		if (ret)
			throw FirstSetException{ret.error()};
		return *ret;
	}

	SymbolsSet find_recursive(Symbol& symbol);

private:
	const Grammar& m_grammar;
	std::unordered_map<Symbol, SymbolsSet> m_firstSet;
};

}	//namespace analyze
