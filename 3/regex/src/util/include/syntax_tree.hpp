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
	[[nodiscard]]
	tl::expected<void, std::string> prase_regex(std::string_view sv);
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
			nodeType { type }, leavePtr { nullptr }, leftChild { std::move(left) },
			rightChild { std::move(right) }
		{
			[[unlikely]] if (nodeType == LEAVE)
				throw std::invalid_argument {
					"LEAVE nodeType must specify idx and char"
				};
		}

		Node(NodeType type, size_t idx, char chr, std::unique_ptr<Node> left = nullptr,
			 std::unique_ptr<Node> right = nullptr):
			nodeType { type }, leavePtr { std::make_unique<Leave>(idx, chr) },
			leftChild { std::move(left) }, rightChild { std::move(right) }
		{
			[[unlikely]] if (nodeType != LEAVE)
				throw std::invalid_argument {
					"NodeType that are not LEAVE cannot initial with idx and chr"
				};
		}
	};

	[[nodiscard]]
	auto parse(std::string_view sv)
		-> tl::expected<std::unique_ptr<Node>, std::string> ;
	[[nodiscard]]	//正常返回匹配括号的后一个字符
	auto pattern_pth(std::string_view sv, size_t idx)
		-> tl::expected<size_t, std::string>;

private:
	std::unique_ptr<Node> m_root;
	//记录每个符号对应编号
	std::unordered_map<char, std::vector<size_t>> m_symbol2idx;
	//记录每个叶子编号对应的字符
	std::unordered_map<size_t, char> m_idx2symbol;

#ifdef DEBUG
	friend class TestSyntaxTree;
	FRIEND_TEST(TestSyntaxTree, test_parse);
	FRIEND_TEST(TestSyntaxTree, test_pattern_pth);
#endif
};

}		//namespace simple_regex

