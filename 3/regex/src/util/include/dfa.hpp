#pragma once

#include <unordered_map>
#include "syntax_tree.hpp"

namespace std
{

template<typename Ty>
struct hash<std::unordered_set<Ty>>
{
	size_t operator()(const std::unordered_set<Ty>& set) const
	{
		size_t hashValue = 0;
		auto hashTy = std::hash<Ty>{};
		for (const auto& elem: set)
		{
			hashValue ^= hashTy(elem);
		}
		return hashValue;
	}
};

}	//namespace std

namespace simple_regex
{

class DFA
{
	using uptr_t = std::unique_ptr<SyntaxTree::Node>;
	using puptr_t = std::unique_ptr<SyntaxTree::Node>*;
	using vertex = std::unordered_set<puptr_t>;
public:
	DFA() = default;
	void create_graph(SyntaxTree&& tree);
	void display_graph(std::ostream& os);

private:	//四个算法函数
	auto cal_nullable(uptr_t& uptr) -> bool;
	template<bool isFirst>
	auto cal_flpos(uptr_t& uptr) -> void;
	auto cal_followpos(uptr_t& uptr) -> void;
	auto construct_graph() -> void;
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

	struct shdPtrVauleHash
	{
		size_t operator()(const std::shared_ptr<vertex>& value) const
		{
			return std::hash<vertex>{}(*value);
		}
	};

	struct shdPtrVauleEqual
	{
		bool operator()(const std::shared_ptr<vertex>& lhs, const std::shared_ptr<vertex>& rhs) const
		{
			return *lhs == *rhs;
		}
	};
private:
	SyntaxTree m_tree;
	std::unordered_map<puptr_t, bool> m_nullable;
	std::unordered_map<puptr_t, std::unordered_set<puptr_t>> m_firstpos;
	std::unordered_map<puptr_t, std::unordered_set<puptr_t>> m_lastpos;
	std::unordered_map<puptr_t, std::unordered_set<puptr_t>> m_followpos;
	
	std::unordered_map<std::shared_ptr<vertex>, bool, shdPtrVauleHash, shdPtrVauleEqual> m_vertexTable;

	std::unordered_map<
		std::shared_ptr<vertex>,
		std::pair<char,  std::shared_ptr<vertex>>,
		shdPtrVauleHash,
		shdPtrVauleEqual
	> m_graph;

#ifdef DEBUG
	friend class TestDFA;
	FRIEND_TEST(TestDFA, test_nullable);
	FRIEND_TEST(TestDFA, test_firstpos);
	FRIEND_TEST(TestDFA, test_lastpos);
	FRIEND_TEST(TestDFA, test_followpos);
	FRIEND_TEST(TestDFA, test_construct_graph);
#endif
};

}	//namespace simple_regex

