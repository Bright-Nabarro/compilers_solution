#pragma once
#include <gtest/gtest.h>
#include "syntax_tree.hpp"

//gtest文档中说明如果使用友元需要将测试函数与类定义在同一命名空间
namespace simple_regex
{

class TestSyntaxTree : public ::testing::Test
{
};

TEST_F(TestSyntaxTree, initial_syntax_tree)
{
	simple_regex::SyntaxTree tree{};
}

TEST_F(TestSyntaxTree, test_pattern_pth)
{
	simple_regex::SyntaxTree tree{};

	// 正常情况
    auto normalRet1 = tree.pattern_pth("()", 0);
    ASSERT_TRUE(normalRet1);
    EXPECT_EQ(*normalRet1, 2);

    auto normalRet2 = tree.pattern_pth("1()3", 1);
    ASSERT_TRUE(normalRet2);
    EXPECT_EQ(*normalRet2, 3);

    auto normalRet3 = tree.pattern_pth("(())abc", 0);
    ASSERT_TRUE(normalRet3);
    EXPECT_EQ(*normalRet3, 4);

    auto normalRet4 = tree.pattern_pth("(1(2)3)def", 0);
    ASSERT_TRUE(normalRet4);
    EXPECT_EQ(*normalRet4, 7);

    auto normalRet5 = tree.pattern_pth("(abc)123", 0);
    ASSERT_TRUE(normalRet5);
    EXPECT_EQ(*normalRet5, 5);

    auto normalRet6 = tree.pattern_pth("(1)(2)xyz", 0);
    ASSERT_TRUE(normalRet6);
    EXPECT_EQ(*normalRet6, 3);

    auto normalRet7 = tree.pattern_pth("(((abc)))ghi", 0);
    ASSERT_TRUE(normalRet7);
    EXPECT_EQ(*normalRet7, 9);

    auto normalRet8 = tree.pattern_pth("(  ) 456", 0);
    ASSERT_TRUE(normalRet8);
    EXPECT_EQ(*normalRet8, 4);

    auto normalRet9 = tree.pattern_pth("( a b c ) end", 0);
    ASSERT_TRUE(normalRet9);
    EXPECT_EQ(*normalRet9, 9);

    auto normalRet10 = tree.pattern_pth("abc(1)done", 3);
    ASSERT_TRUE(normalRet10);
    EXPECT_EQ(*normalRet10, 6);

	//异常
	//assert
	//auto expectRet0 = tree.pattern_pth("1", 0);
	auto expectRet1 = tree.pattern_pth("(", 0);
	ASSERT_FALSE(expectRet1);
	 // 部分匹配但有剩余的左括号
    auto expectRet2 = tree.pattern_pth("(abc(", 0);
    ASSERT_FALSE(expectRet2);
    // 嵌套括号未匹配
    auto expectRet3 = tree.pattern_pth("((abc)", 0);
    ASSERT_FALSE(expectRet3);
    // 没有匹配的括号，括号在文字中间
    auto expectRet4 = tree.pattern_pth("abc(", 3);
    ASSERT_FALSE(expectRet4);
}	

TEST_F(TestSyntaxTree, test_parse)
{
	//正常情况
	{
		simple_regex::SyntaxTree tree{};
		auto normalRet0 = tree.parse("");
		ASSERT_TRUE(normalRet0);
		EXPECT_EQ(*normalRet0, nullptr);
	}

	{
		simple_regex::SyntaxTree tree{};
		auto normalRet1 = tree.parse("a");
		ASSERT_TRUE(normalRet1);
		EXPECT_EQ((*normalRet1)->nodeType, SyntaxTree::Node::LEAVE);
		ASSERT_NE((*normalRet1)->leavePtr, nullptr);
		EXPECT_EQ((*normalRet1)->leftChild, nullptr);
		EXPECT_EQ((*normalRet1)->rightChild, nullptr);
		EXPECT_EQ((*normalRet1)->leavePtr->chr, 'a');
		EXPECT_EQ((*normalRet1)->leavePtr->idx, 0);
	}
	
	//验证基础情况
	//cat
	{
		simple_regex::SyntaxTree tree{};
		auto normalRet2 = tree.parse("ab");
		ASSERT_TRUE(normalRet2);
		EXPECT_EQ((*normalRet2)->nodeType, SyntaxTree::Node::CAT);
		auto& normalRet2Left = (*normalRet2)->leftChild;
		auto& normalRet2Right = (*normalRet2)->rightChild;
		ASSERT_EQ((*normalRet2)->leavePtr, nullptr);
		ASSERT_NE((*normalRet2)->leftChild, nullptr);
		ASSERT_NE((*normalRet2)->rightChild, nullptr);
		EXPECT_EQ(normalRet2Left->nodeType, SyntaxTree::Node::LEAVE);
		ASSERT_NE(normalRet2Left->leavePtr, nullptr);
		EXPECT_EQ(normalRet2Left->leavePtr->chr, 'a');
		EXPECT_EQ(normalRet2Right->nodeType, SyntaxTree::Node::LEAVE);
		ASSERT_NE(normalRet2Right->leavePtr, nullptr);
		EXPECT_EQ(normalRet2Right->leavePtr->chr, 'b');
	}
	
	//or
	{
		simple_regex::SyntaxTree tree{};
		auto normalRet3 = tree.parse("a|b");
		ASSERT_TRUE(normalRet3);
		EXPECT_EQ((*normalRet3)->nodeType, SyntaxTree::Node::OR);
		auto& normalRet3Left = (*normalRet3)->leftChild;
		auto& normalRet3Right = (*normalRet3)->rightChild;
		ASSERT_EQ((*normalRet3)->leavePtr, nullptr);
		ASSERT_NE((*normalRet3)->leftChild, nullptr);
		ASSERT_NE((*normalRet3)->rightChild, nullptr);
		EXPECT_EQ(normalRet3Left->nodeType, SyntaxTree::Node::LEAVE);
		ASSERT_NE(normalRet3Left->leavePtr, nullptr);
		EXPECT_EQ(normalRet3Left->leavePtr->chr, 'a');
		EXPECT_EQ(normalRet3Right->nodeType, SyntaxTree::Node::LEAVE);
		ASSERT_NE(normalRet3Right->leavePtr, nullptr);
		EXPECT_EQ(normalRet3Right->leavePtr->chr, 'b');
	}
	
	//star
	{
		simple_regex::SyntaxTree tree{};
		auto ret = tree.parse("a*");
		ASSERT_TRUE(ret);
		EXPECT_EQ((*ret)->nodeType, SyntaxTree::Node::STAR);
		auto& left = (*ret)->leftChild;
		auto& right = (*ret)->rightChild;
		ASSERT_EQ((*ret)->leavePtr, nullptr);
		ASSERT_NE(left, nullptr);
		ASSERT_EQ(right, nullptr);
		EXPECT_EQ(left->nodeType, SyntaxTree::Node::LEAVE);
		ASSERT_NE(left->leavePtr, nullptr);
		EXPECT_EQ(left->leavePtr->chr, 'a');
		EXPECT_EQ(left->leavePtr->idx, 0);
	}
	// 测试括号综合情况
	{
		simple_regex::SyntaxTree tree{};
		auto ret = tree.parse("()");
		ASSERT_EQ(*ret, nullptr);
	}
	{
		simple_regex::SyntaxTree tree{};
		auto ret = tree.parse("(a)");
		ASSERT_TRUE(ret);
		EXPECT_EQ((*ret)->nodeType, SyntaxTree::Node::LEAVE);
		ASSERT_NE((*ret)->leavePtr, nullptr);
		EXPECT_EQ((*ret)->leavePtr->chr, 'a');
		EXPECT_EQ((*ret)->leavePtr->idx, 0);
	}
	{
		simple_regex::SyntaxTree tree{};
		auto ret = tree.parse("(ab)");
		ASSERT_TRUE(ret);
		EXPECT_EQ((*ret)->nodeType, SyntaxTree::Node::CAT);
		auto& pret = *ret;
		auto& left = (*ret)->leftChild;
		auto& right = (*ret)->rightChild;
		ASSERT_EQ(pret->leavePtr, nullptr);
		ASSERT_NE(pret->leftChild, nullptr);
		ASSERT_NE(pret->rightChild, nullptr);
		EXPECT_EQ(left->nodeType, SyntaxTree::Node::LEAVE);
		ASSERT_NE(left->leavePtr, nullptr);
		EXPECT_EQ(left->leavePtr->chr, 'a');
		EXPECT_EQ(left->leavePtr->idx, 0);
		EXPECT_EQ(right->nodeType, SyntaxTree::Node::LEAVE);
		ASSERT_NE(right->leavePtr, nullptr);
		EXPECT_EQ(right->leavePtr->chr, 'b');	
		EXPECT_EQ(right->leavePtr->idx, 1);
	}
	{
		simple_regex::SyntaxTree tree{};
		auto ret = tree.parse("(a)(b)");
		ASSERT_TRUE(ret);
		EXPECT_EQ((*ret)->nodeType, SyntaxTree::Node::CAT);
		auto& pret = *ret;
		auto& left = (*ret)->leftChild;
		auto& right = (*ret)->rightChild;
		ASSERT_EQ(pret->leavePtr, nullptr);
		ASSERT_NE(pret->leftChild, nullptr);
		ASSERT_NE(pret->rightChild, nullptr);
		EXPECT_EQ(left->nodeType, SyntaxTree::Node::LEAVE);
		ASSERT_NE(left->leavePtr, nullptr);
		EXPECT_EQ(left->leavePtr->chr, 'a');
		EXPECT_EQ(left->leavePtr->idx, 0);
		EXPECT_EQ(right->nodeType, SyntaxTree::Node::LEAVE);
		ASSERT_NE(right->leavePtr, nullptr);
		EXPECT_EQ(right->leavePtr->chr, 'b');	
		EXPECT_EQ(right->leavePtr->idx, 1);
	}

}	

} //namespace reg_test
