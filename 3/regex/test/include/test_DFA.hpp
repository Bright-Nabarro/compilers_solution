#pragma once
#include <gtest/gtest.h>
#include "dfa.hpp"
namespace simple_regex
{

class TestDFA: public ::testing::Test
{
};

TEST_F(TestDFA, initial)
{
	DFA d{};
}

TEST_F(TestDFA, test_nullable)
{
	{
		SyntaxTree tree{};
		auto _ = tree.parse_regex("");
		DFA dfa {};
		dfa.create_graph(std::move(tree));
	}
	{
		SyntaxTree tree{};
		auto _ = tree.parse_regex("ab");
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_nullable.size(), 5);
		auto& treeRoot = dfa.m_tree.m_root;
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->rightChild)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(
			&treeRoot->leftChild->leftChild)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(
			&treeRoot->leftChild->rightChild)->second, false);
	}
	{
		SyntaxTree tree{};
		auto _ = tree.parse_regex("a*");
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_nullable.size(), 4);
		auto& treeRoot = dfa.m_tree.m_root;
		//initial
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->rightChild)->second, false);
		//add
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild)->second, true);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->leftChild)->second, false);
	}
	{
		SyntaxTree tree{};
		auto _ = tree.parse_regex("a|b");
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_nullable.size(), 5);
		auto& treeRoot = dfa.m_tree.m_root;
		//initial
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->rightChild)->second, false);
		//add
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->leftChild)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->rightChild)->second, false);
	}
	{
		SyntaxTree tree{};
		auto _ = tree.parse_regex("b|(a*)");
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_nullable.size(), 6);
		auto& treeRoot = dfa.m_tree.m_root;
		//initial
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->rightChild)->second, false);
		//add
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild)->second, true);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->leftChild)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->rightChild)->second, true);
	}
	{
		SyntaxTree tree{};
		auto _ = tree.parse_regex("b(a*)");
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_nullable.size(), 6);
		auto& treeRoot = dfa.m_tree.m_root;
		//initial
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->rightChild)->second, false);
		//add
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->leftChild)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->rightChild)->second, true);
	}
	{
		SyntaxTree tree{};
		auto _ = tree.parse_regex("(b*)(a*)");
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_nullable.size(), 7);
		auto& treeRoot = dfa.m_tree.m_root;
		//initial
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot)->second, false);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->rightChild)->second, false);
		//add
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild)->second, true);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->leftChild)->second, true);
		EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->rightChild)->second, true);
	}
	{
	    // 测试正则表达式 "a(b|c*)"
	    SyntaxTree tree{};
	    auto _ = tree.parse_regex("a(b|c*)");
	    DFA dfa {};
	    dfa.create_graph(std::move(tree));
	    
	    EXPECT_EQ(dfa.m_nullable.size(), 8);
	    auto& treeRoot = dfa.m_tree.m_root;
	
	    EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->rightChild)->second, true);
	    EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->leftChild)->second, false);
	}
	{
	    // 测试正则表达式 "((a*)|b)c"
	    SyntaxTree tree{};
	    auto _ = tree.parse_regex("((a*)|b)c");
	    DFA dfa {};
	    dfa.create_graph(std::move(tree));
	
	    EXPECT_EQ(dfa.m_nullable.size(), 8);
	    auto& treeRoot = dfa.m_tree.m_root;
	
	    EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->leftChild)->second, true);
	    EXPECT_EQ(dfa.m_nullable.find(&treeRoot->leftChild->rightChild)->second, false);
	}
}

TEST_F(TestDFA, test_firstpos)
{
	{
		SyntaxTree tree{};
		auto _ = tree.parse_regex("a");
		DFA dfa {};
		dfa.create_graph(std::move(tree));
	    EXPECT_EQ(dfa.m_firstpos.size(), 3);
		auto& leave = dfa.m_tree.m_root->leftChild;
		auto& leaveSet = dfa.m_firstpos.find(&leave)->second;
		EXPECT_EQ(leaveSet.size(), 1);
		EXPECT_TRUE(leaveSet.contains(&leave));
		auto& rootSet = dfa.m_firstpos.find(&dfa.m_tree.m_root)->second;
		EXPECT_TRUE(rootSet.contains(&leave));
	}
	{
		SyntaxTree tree{};
		auto _ = tree.parse_regex("ab");
		DFA dfa {};
		dfa.create_graph(std::move(tree));
	    EXPECT_EQ(dfa.m_firstpos.size(), 5);
		auto& oriRoot = dfa.m_tree.m_root->leftChild;
		auto& oriRootSet = dfa.m_firstpos.find(&oriRoot)->second;
		auto& oriLeftLeave = oriRoot->leftChild;
		EXPECT_TRUE(oriRootSet.contains(&oriLeftLeave));
	}
	{
	    SyntaxTree tree{};
	    auto _ = tree.parse_regex("a*");
	    DFA dfa {};
	    dfa.create_graph(std::move(tree));
	    
	    EXPECT_EQ(dfa.m_firstpos.size(), 4); // 检查 firstpos 的大小
	    auto& root = dfa.m_tree.m_root;
	    auto& rootSet = dfa.m_firstpos.find(&root)->second;
	    
	    auto& starChild = root->leftChild->leftChild; // a 节点
	    EXPECT_EQ(rootSet.size(), 1); // 检查根节点的 firstpos
	    EXPECT_TRUE(rootSet.contains(&starChild)); // firstpos 应包含 a 的叶子节点
	}
	{
	    SyntaxTree tree{};
	    auto _ = tree.parse_regex("a|b");
	    DFA dfa {};
	    dfa.create_graph(std::move(tree));
	    
	    EXPECT_EQ(dfa.m_firstpos.size(), 5); // 包含根节点和叶子节点
	    auto& root = dfa.m_tree.m_root;
	    auto& leftChild = root->leftChild->leftChild; // a 节点
	    auto& rightChild = root->leftChild->rightChild; // b 节点
	    
	    auto& rootSet = dfa.m_firstpos.find(&root)->second;
	    EXPECT_TRUE(rootSet.contains(&leftChild));
	    EXPECT_TRUE(rootSet.contains(&rightChild));
	    
	    auto& leftSet = dfa.m_firstpos.find(&leftChild)->second;
	    EXPECT_TRUE(leftSet.contains(&leftChild));
	    
	    auto& rightSet = dfa.m_firstpos.find(&rightChild)->second;
	    EXPECT_TRUE(rightSet.contains(&rightChild));
	}
	{
	    SyntaxTree tree{};
	    auto _ = tree.parse_regex("(a|b)*");
	    DFA dfa {};
	    dfa.create_graph(std::move(tree));
	
	    EXPECT_EQ(dfa.m_firstpos.size(), 6); // 包含根节点和叶子节点
	    auto& oriRoot = dfa.m_tree.m_root->leftChild;
	    auto& leftChild = oriRoot->leftChild->leftChild; // a 节点
	    auto& rightChild = oriRoot->leftChild->rightChild; // b 节点
	
	    auto& oriRootSet = dfa.m_firstpos.find(&oriRoot)->second;
	    EXPECT_TRUE(oriRootSet.contains(&leftChild));
	    EXPECT_TRUE(oriRootSet.contains(&rightChild));
	}
	{
	    SyntaxTree tree{};
	    auto _ = tree.parse_regex("a(b|c)");
	    DFA dfa {};
	    dfa.create_graph(std::move(tree));
	
	    EXPECT_EQ(dfa.m_firstpos.size(), 7);
	    auto& root = dfa.m_tree.m_root;
	    auto& leftChild = root->leftChild->leftChild; // a 节点
	    auto& rightChild = root->leftChild->rightChild; // b|c 的根节点
	
	    auto& rootSet = dfa.m_firstpos.find(&root)->second;
	    EXPECT_TRUE(rootSet.contains(&leftChild));
	    
	    auto& rightChildLeft = rightChild->leftChild;
	    EXPECT_TRUE(dfa.m_firstpos.find(&rightChild)->second.contains(&rightChildLeft));
	}
	{
	    SyntaxTree tree{};
	    auto _ = tree.parse_regex("(a*)b");
	    DFA dfa {};
	    dfa.create_graph(std::move(tree));
	
	    EXPECT_EQ(dfa.m_firstpos.size(), 6);
	    auto& root = dfa.m_tree.m_root;
	    auto& leftChild = root->leftChild->leftChild;		// a* 的节点
	    auto& rightChild = root->leftChild->rightChild;		// b 的节点
	
	    auto& rootSet = dfa.m_firstpos.find(&root)->second;
	    EXPECT_TRUE(rootSet.contains(&leftChild->leftChild));
		EXPECT_TRUE(rootSet.contains(&rightChild));
	
	    auto& leftSet = dfa.m_firstpos.find(&leftChild)->second;
	    EXPECT_TRUE(leftSet.contains(&leftChild->leftChild));
	
	    auto& rightSet = dfa.m_firstpos.find(&rightChild)->second;
	    EXPECT_TRUE(rightSet.contains(&rightChild));
	}

}

}	//namespace simple_regex