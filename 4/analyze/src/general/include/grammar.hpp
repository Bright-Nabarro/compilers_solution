#pragma once
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "tl/expected.hpp"
#include "symbol.hpp"

namespace analyze
{

struct SymbolListHash
{
	/// 假定vector小于size_t的位数
	std::size_t operator()(const std::vector<Symbol>& vec) const
	{
		if (vec.empty())
			return 0;

		auto hash_func = std::hash<Symbol>{};
		std::size_t ret = hash_func(vec[0]);
		for (size_t i = 0; i < vec.size(); ++i)
		{
			ret <<= 1;
			ret ^= hash_func(vec[i]);
		}
		return ret;
	}
};

class IGrammarParser;

class Grammar
{

public:
	using SymbolsSet = std::unordered_set<std::vector<Symbol>, SymbolListHash>;
	inline static const Symbol empty_symbol { Symbol::other, "e" };
	inline static const std::vector empty_right { empty_symbol };
public:
	Grammar() = default;
	virtual ~Grammar() = default;
	[[nodiscard]]
	tl::expected<Symbol, std::string> get_start() const;
	[[nodiscard]]
	Symbol get_start_unchecked() const;
	[[nodiscard]]
	auto find(const Symbol& symbol) const
		-> tl::expected<std::reference_wrapper<const SymbolsSet>, std::string>;
	/// 如果重复插入相同的键值对，则返回false
	[[nodiscard]]
	bool add_rule(Symbol left, std::vector<Symbol> right);
	/// 
	void set_start(Symbol start_symbol);
	void display_latex(std::ostream& os, bool markdown = true) const;
	/// 检查所有no_terminal是否都作为左部出现, 如果start_symbol未设置，返回false
	[[nodiscard]]
	bool validate_noterminals() const;
	/// 如果m_nullable没有初始化，抛出异常
	[[nodiscard]]
	bool nullable(const Symbol& symbol) const;
	/*
	 * 检测所有的no_terminal是否可达空，调用此函数之前需要调用validate_noterminals
	 * 提供近似`O(N+R)`时间复杂度，其中N为文法no_terminal数量，R为规则数
	 */
	void infer_empty_string();
private:
	/*
	 * 递归遍历所有vector.size() == 1的规则
	 * 所有一开始遍历到的元素先在m_nullable上做初始化，
	 * 递归前检查，以免死循环
	 */
	bool infer_empty_string(const std::vector<Symbol>& right);
private:
	std::optional<Symbol> m_start_symbol;
	std::unordered_map<Symbol, SymbolsSet> m_rules;
	/// 只存储no_terminal
	std::unordered_map<Symbol, bool> m_nullable;
};



class IGrammarParser
{
public:
	/*
	 * 解析时进行检查，如果出错返回错误描述
	 * 检查内容：
	 * - file_path是否合法
	 * - terminals, productions是否存在于配置文件中 (如果start_symbol域不存在，则默认为第一个规则左部)
	 * - 开始符号是否存在于文法规则左部
	 * - 
	 */

	[[nodiscard]]
	virtual tl::expected<Grammar, std::string> parse(std::istream& in) = 0;
	virtual ~IGrammarParser() = default;
};

class YamlParser: public IGrammarParser
{
public:
	YamlParser() = default;
	[[nodiscard]]
	tl::expected<Grammar, std::string> parse(std::istream& in) override;
private:
	template<typename Ty> [[nodiscard]]
	static tl::expected<Ty, std::string> nothrow_as(const YAML::Node& node);
	//有两层，第一层是所有对应规则集合，第二层是Symbol集合
	auto parse_rhs(Symbol& left, Grammar& grammar, const YAML::Node& rhs_set,
				   const std::unordered_set<std::string>& terminal_string_set) const
		-> tl::expected<void, std::string>;

};

}		//namespace analyze


