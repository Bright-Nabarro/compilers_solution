#pragma once
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <tl/expected.hpp>
#include <unordered_map>
#include <unordered_set>

namespace simple_regex
{

class SyntaxTree
{
	friend class DFA;
public:
	SyntaxTree(const SyntaxTree&) = delete;
	SyntaxTree& operator=(const SyntaxTree&) = delete;
	SyntaxTree(SyntaxTree&&) noexcept = default;
	SyntaxTree& operator=(SyntaxTree&&) noexcept = default;
	virtual ~SyntaxTree() = default;

	SyntaxTree() : m_root { nullptr }{};
	[[nodiscard]]	//每次调用重置内部状态
	tl::expected<void, std::string> parse_regex(std::string_view sv);
	void display(std::ostream& os) const;
	
private:
	struct Leave
	{
		char chr;
	};

	struct Node
	{
		enum NodeType {
			CAT, OR, STAR, LEAVE, END
		};
		NodeType nodeType;
		std::unique_ptr<Leave> leavePtr;
		std::unique_ptr<Node> leftChild;
		std::unique_ptr<Node> rightChild;

		Node(NodeType type, std::unique_ptr<Node> left = nullptr,
			 std::unique_ptr<Node> right = nullptr):
			nodeType { type },
			leavePtr { nullptr }, 
			leftChild { std::move(left) },
			rightChild { std::move(right) }
		{
			if (nodeType == LEAVE || nodeType == END) [[unlikely]] 
				throw std::invalid_argument {
					"LEAVE nodeType must specify idx and char"
				};
		}

		Node(NodeType type, char chr, std::unique_ptr<Node> left = nullptr,
			 std::unique_ptr<Node> right = nullptr):
			nodeType { type }, leavePtr { std::make_unique<Leave>(chr) },
			leftChild { std::move(left) }, rightChild { std::move(right) }
		{
			if (nodeType != LEAVE && nodeType != END) [[unlikely]]
				throw std::invalid_argument {
					"NodeType that are not LEAVE cannot initial with idx and chr"
				};
		}
	};

	[[nodiscard]]
	auto parse(std::string_view sv)
		-> tl::expected<std::unique_ptr<Node>, std::string>;
	[[nodiscard]]	//parse递归辅助函数
	auto parse_cat(std::string_view sv, size_t leftBeg, size_t leftLen,
		size_t rightBeg, size_t rightLen, Node::NodeType type)
		-> tl::expected<std::unique_ptr<Node>, std::string>;
	[[nodiscard]]	//正常返回匹配括号的后一个字符
	auto pattern_pth(std::string_view sv, size_t idx) const
		-> tl::expected<size_t, std::string>;

	void reset();
	
private:
	mutable struct
	{
		size_t catCounter = 0;
		size_t orCounter = 0;
		size_t starCounter = 0;
		size_t leavesCounter = 0;
	} displayContext;
	void display(std::ostream& os, const std::unique_ptr<Node>& ptr,
			const std::string& ptrStr) const;
	auto nodeType2string(const std::unique_ptr<Node>& ptr) const ->
		std::optional<std::pair<std::string, std::string>>;
	std::unique_ptr<Node> m_root;
	std::unordered_set<std::unique_ptr<Node>*> m_leavesTable;
	std::unordered_map<char, std::vector<std::unique_ptr<Node>*>> m_chrTable;

#ifdef DEBUG
	friend class TestSyntaxTree;
	FRIEND_TEST(TestSyntaxTree, test_parse);
	FRIEND_TEST(TestSyntaxTree, test_pattern_pth);
	FRIEND_TEST(TestSyntaxTree, test_parse_regex);
	FRIEND_TEST(TestDFA, test_nullable);
	FRIEND_TEST(TestDFA, test_firstpos);
	FRIEND_TEST(TestDFA, test_lastpos);
	FRIEND_TEST(TestDFA, test_followpos);
#endif
};

}		//namespace simple_regex

