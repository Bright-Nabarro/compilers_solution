#pragma once
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <tl/expected.hpp>
#include <unordered_map>
#include <gtest/gtest.h>

namespace simple_regex
{

class SyntaxTree
{
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
		size_t idx;
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

		Node(NodeType type, char chr, size_t idx, std::unique_ptr<Node> left = nullptr,
			 std::unique_ptr<Node> right = nullptr):
			nodeType { type }, leavePtr { std::make_unique<Leave>(idx, chr) },
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
	} displayContext;
	void display(std::ostream& os, const std::unique_ptr<Node>& ptr,
			const std::string& ptrStr) const;
	auto nodeType2string(const std::unique_ptr<Node>& ptr) const ->
		std::optional<std::pair<std::string, std::string>>;
	std::unique_ptr<Node> m_root;
	//记录每个符号对应编号
	std::unordered_map<char, std::vector<size_t>> m_symbol2idx;
	//记录每个叶子编号对应的字符
	std::unordered_map<size_t, char> m_idx2symbol;
	size_t m_leavesCounter = 0;

#ifdef DEBUG
	friend class TestSyntaxTree;
	FRIEND_TEST(TestSyntaxTree, test_parse);
	FRIEND_TEST(TestSyntaxTree, test_pattern_pth);
	FRIEND_TEST(TestSyntaxTree, test_parse_regex);
#endif
};

}		//namespace simple_regex

