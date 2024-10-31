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
            hashValue ^= hashTy(elem) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
		}
		return hashValue;
	}
};

}	//namespace std

namespace simple_regex
{

class DFA
{
public:
	using sptr_t = std::shared_ptr<SyntaxTree::Node>;
	using vertex = std::unordered_set<sptr_t>;
	DFA() = default;
	DFA(const DFA&) = delete;
	DFA& operator= (const DFA&) = delete;
	DFA(DFA&&) = default;
	DFA& operator= (DFA&&) = default;
	void create_graph(SyntaxTree&& tree);
	void display_graph(std::ostream& os) const;
	void display_followpos(std::ostream& os) const;
	const auto& get_graph() const
	{ return m_graph; }
	auto get_begin() const
	{ return m_begin; }
	const auto& get_vtxTable() const
	{ return m_vertexTable; }
	//最小化dfa
	void minimize();

private:	//四个算法函数
	auto cal_nullable(sptr_t uptr) -> bool;
	template<bool isFirst>
	auto cal_flpos(sptr_t uptr) -> void;
	auto cal_followpos(sptr_t uptr) -> void;
	auto construct_graph() -> void;
private:	//辅助函数
	[[nodiscard]]
	auto check_and_get_table_elements(sptr_t sptr, const auto& table) const
	{
		auto retPtr = table.find(sptr);
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

	struct pairShdPtrCharHash
	{
		size_t operator()(const std::pair<std::shared_ptr<vertex>, char>& value) const
		{
			return shdPtrVauleHash{}(value.first) ^ value.second;
		}
	};

	struct pairShdPtrCharEqual
	{
		bool operator()(const std::pair<std::shared_ptr<vertex>, char>& lhs,
			const std::pair<std::shared_ptr<vertex>, char>& rhs) const
		{
			return shdPtrVauleEqual{}(lhs.first, rhs.first)
				&& lhs.second == rhs.second;
		}
	};
	using ShdVertex = std::shared_ptr<vertex>;
	using State = std::unordered_set<ShdVertex, shdPtrVauleHash, shdPtrVauleEqual> ;
	using StateDivision = std::vector<State>;
	auto initial_division() -> StateDivision;
	auto get_next(ShdVertex, char) -> ShdVertex;
	
	//auto new_state(ShdVertex shdVertex) 

private:

	SyntaxTree m_tree;
	std::unordered_map<sptr_t, bool> m_nullable;
	std::unordered_map<sptr_t, std::unordered_set<sptr_t>> m_firstpos;
	std::unordered_map<sptr_t, std::unordered_set<sptr_t>> m_lastpos;
	std::unordered_map<sptr_t, std::unordered_set<sptr_t>> m_followpos;
	//value为true代表此节点为接受状态
    std::unordered_map<std::shared_ptr<vertex>, bool, shdPtrVauleHash,
                           shdPtrVauleEqual>
        m_vertexTable;

    std::unordered_map<
		std::pair<std::shared_ptr<vertex>, char>,
		std::shared_ptr<vertex>,
		pairShdPtrCharHash,
		pairShdPtrCharEqual
	> m_graph;
	std::shared_ptr<vertex> m_begin;

#ifdef DEBUG
	friend class TestDFA;
	FRIEND_TEST(TestDFA, test_nullable);
	FRIEND_TEST(TestDFA, test_firstpos);
	FRIEND_TEST(TestDFA, test_lastpos);
	FRIEND_TEST(TestDFA, test_followpos);
	FRIEND_TEST(TestDFA, test_construct_graph);
	FRIEND_TEST(TestDFA, test_end);
#endif
};

}	//namespace simple_regex

