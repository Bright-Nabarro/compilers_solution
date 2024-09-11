#pragma once

#include <unordered_map>
#include <vector>
#include "syntax_tree.hpp"

namespace simple_regex
{

class DFA
{
	using uptr_t = std::unique_ptr<SyntaxTree::Node>;
	using puptr_t = std::unique_ptr<SyntaxTree::Node>*;
public:
	DFA() = default;
	void create_graph(SyntaxTree&& tree);
	void display(std::ostream& os);

private:	//四个算法函数
	auto cal_nullable(uptr_t& uptr) -> bool;
	template<bool isFirst>
	auto cal_flpos(uptr_t& uptr) -> void;
	auto cal_followpos(uptr_t& uptr) -> void;
private:	//辅助函数
	[[nodiscard]]
	auto check_and_get_table_elements(puptr_t puptr, const auto& table) const
	{
		auto retPtr = table.find(puptr);
		if (retPtr == table.end())	[[unlikely]]
			throw std::logic_error {
				"cannot find ptr in table"
			};
		return retPtr;
	}
private:
	SyntaxTree m_tree;
	std::unordered_map<puptr_t, bool> m_nullable;
	std::unordered_map<puptr_t, std::unordered_set<puptr_t>> m_firstpos;
	std::unordered_map<puptr_t, std::unordered_set<puptr_t>> m_lastpos;
	std::unordered_map<puptr_t, std::unordered_set<puptr_t>> m_followpos;
	//储存图的邻接矩阵
	std::unordered_map<puptr_t, std::vector<puptr_t>> m_graph;

#ifdef DEBUG
	friend class TestDFA;
	FRIEND_TEST(TestDFA, test_nullable);
	FRIEND_TEST(TestDFA, test_firstpos);
	FRIEND_TEST(TestDFA, test_followpos);
#endif
};

}	//namespace simple_regex

