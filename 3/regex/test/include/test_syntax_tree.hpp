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
	simple_regex::SyntaxTree tree{};
	//正常情况
	auto normalRet1 = tree.parse("a");
	ASSERT_TRUE(normalRet1);
	EXPECT_EQ((*normalRet1)->nodeType, SyntaxTree::Node::LEAVE);
	ASSERT_NE((*normalRet1)->leavePtr, nullptr);
	EXPECT_EQ((*normalRet1)->leftChild, nullptr);
	EXPECT_EQ((*normalRet1)->rightChild, nullptr);
	EXPECT_EQ((*normalRet1)->leavePtr->chr, 'a');

}	

} //namespace reg_test
