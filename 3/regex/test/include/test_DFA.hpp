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
	    EXPECT_EQ(rootSet.size(), 2); // 检查根节点的 firstpos
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
		auto& oriRoot = root->leftChild;
	    auto& leftChild = oriRoot->leftChild;		// a* 的节点
	    auto& rightChild = oriRoot->rightChild;		// b 的节点
	
	    auto& oriRootSet = dfa.m_firstpos.find(&oriRoot)->second;
	    EXPECT_TRUE(oriRootSet.contains(&leftChild->leftChild));
		EXPECT_TRUE(oriRootSet.contains(&rightChild));
	
	    auto& leftSet = dfa.m_firstpos.find(&leftChild)->second;
	    EXPECT_TRUE(leftSet.contains(&leftChild->leftChild));
	
	    auto& rightSet = dfa.m_firstpos.find(&rightChild)->second;
	    EXPECT_TRUE(rightSet.contains(&rightChild));
	}
	{
	    SyntaxTree tree{};		//       v  v v
	    auto _ = tree.parse_regex("(((a*)b)*c)d");
	    DFA dfa {};
	    dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_nullable.size(), 11);
		EXPECT_EQ(dfa.m_firstpos.size(), 11);
		auto& oriRoot = dfa.m_tree.m_root->leftChild;
		//auto& d = oriRoot->rightChild;
		auto& cat1 = oriRoot->leftChild;
		auto& c = cat1->rightChild;
		auto& star1 = cat1->leftChild;
		auto& cat2 = star1->leftChild;
		auto& b = cat2->rightChild;
		auto& star2 = cat2->leftChild;
		auto& a = star2->leftChild;

		auto& aSet = dfa.m_firstpos.find(&a)->second;
		EXPECT_TRUE(aSet.contains(&a));
		EXPECT_EQ(aSet.size(), 1);
		auto& star2Set = dfa.m_firstpos.find(&star2)->second;
		EXPECT_EQ(aSet, star2Set);
		auto& cat2Set = dfa.m_firstpos.find(&cat2)->second;
		EXPECT_EQ(cat2Set.size(), 2);
		EXPECT_TRUE(cat2Set.contains(&a));
		EXPECT_TRUE(cat2Set.contains(&b));
		auto& cat1Set = dfa.m_firstpos.find(&cat1)->second;
		EXPECT_EQ(cat1Set.size(), 3);
		EXPECT_TRUE(cat1Set.contains(&a));
		EXPECT_TRUE(cat1Set.contains(&b));
		EXPECT_TRUE(cat1Set.contains(&c));
		auto& oriRootSet = dfa.m_firstpos.find(&oriRoot)->second;
		EXPECT_EQ(oriRootSet.size(), 3);
		EXPECT_TRUE(oriRootSet.contains(&a));
		EXPECT_TRUE(oriRootSet.contains(&b));
		EXPECT_TRUE(oriRootSet.contains(&c));

	}
}

TEST_F(TestDFA, test_lastpos)
{
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("a");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_lastpos.size(), 3);
		auto& root = dfa.m_tree.m_root;
		auto& rLeave = root->rightChild;
		auto& rLeaveSet = dfa.m_lastpos.find(&rLeave)->second;
		EXPECT_EQ(rLeaveSet.size(), 1);
		EXPECT_TRUE(rLeaveSet.contains(&rLeave));
		auto& rootSet = dfa.m_lastpos.find(&root)->second;
		EXPECT_TRUE(rootSet.contains(&rLeave));
		EXPECT_EQ(rootSet.size(), 1);
	}
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("ab");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_lastpos.size(), 5);
		auto& root = dfa.m_tree.m_root;
		auto& endLeave = root->rightChild;
		auto& rootSet = dfa.m_lastpos.find(&root)->second;
		EXPECT_EQ(rootSet.size(), 1);
		EXPECT_TRUE(rootSet.contains(&endLeave));
		auto& lLeave = root->leftChild;
		auto& lSet = dfa.m_lastpos.find(&lLeave)->second;
		auto& b = lLeave->rightChild;
		EXPECT_EQ(lSet.size(), 1);
		EXPECT_TRUE(lSet.contains(&b));
	}
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("a|b");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_lastpos.size(), 5);
		auto& root = dfa.m_tree.m_root;
		auto& or1 = root->leftChild;
		auto& a = or1->leftChild;
		auto& b = or1->rightChild;
		auto& lastOr1 = dfa.m_lastpos.find(&or1)->second;
		EXPECT_EQ(lastOr1.size(), 2);
		EXPECT_TRUE(lastOr1.contains(&a));
		EXPECT_TRUE(lastOr1.contains(&b));
	}
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("a*");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_lastpos.size(), 4);
		auto& root = dfa.m_tree.m_root;
		auto& rootSet = dfa.m_lastpos.find(&root)->second;
		auto& endLeave = root->rightChild;
		EXPECT_EQ(rootSet.size(), 1);
		EXPECT_TRUE(rootSet.contains(&endLeave));
		auto& star1 = root->leftChild;
		auto& a = star1->leftChild;
		auto& star1Set = dfa.m_lastpos.find(&star1)->second;
		EXPECT_EQ(star1Set.size(), 1);
		EXPECT_TRUE(star1Set.contains(&a));
	}
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("ba*");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_lastpos.size(), 6);
		auto& root = dfa.m_tree.m_root;
		auto& cat1 = root->leftChild;
		auto& b = cat1->leftChild;
		auto& star1 = cat1->rightChild;
		auto& a = star1->leftChild;
		auto& cat1Set = dfa.m_lastpos.find(&cat1)->second;
		EXPECT_EQ(cat1Set.size(), 2);
		EXPECT_TRUE(cat1Set.contains(&a));
		EXPECT_TRUE(cat1Set.contains(&b));
	}

	
	{
	    SyntaxTree tree;
	    auto treeRet = tree.parse_regex("c|ab");
	    ASSERT_TRUE(treeRet);
	
	    DFA dfa {};
	    dfa.create_graph(std::move(tree));
	
	    // 期望节点个数为6： cat根节点，或运算符节点，a和ab字符节点，右子节点
	    EXPECT_EQ(dfa.m_lastpos.size(), 7);
	
	    // 检查树的结构
	    auto& root = dfa.m_tree.m_root;
	    auto& or1 = root->leftChild;
	    auto& c = or1->leftChild;
	    auto& cat1 = or1->rightChild;
	    auto& a = cat1->leftChild;
	    auto& b = cat1->rightChild;
	
	    // 检查lastpos集，优先匹配 a
	    auto& or1Set = dfa.m_lastpos.find(&or1)->second;
	    EXPECT_EQ(or1Set.size(), 2);
	    EXPECT_TRUE(or1Set.contains(&c));
	    EXPECT_TRUE(or1Set.contains(&a));
		EXPECT_TRUE(!or1Set.contains(&b));
	}

	{
	    SyntaxTree tree;
	    auto treeRet = tree.parse_regex("(ab|c)d");
	    ASSERT_TRUE(treeRet);
	
	    DFA dfa {};
	    dfa.create_graph(std::move(tree));
	
	    // 期望节点个数为7： cat根节点，连接符，或运算符节点，ab和a字符节点，c字符节点，右子节点
	    EXPECT_EQ(dfa.m_lastpos.size(), 9);
	
	    // 检查树的结构
	    auto& root = dfa.m_tree.m_root;
	    auto& cat1 = root->leftChild;
	    auto& or1 = cat1->leftChild;
	    auto& cat2 = or1->leftChild;
	    auto& a = cat2->leftChild;
	    //auto& b = cat2->rightChild;
	    auto& c = or1->rightChild;
	    auto& d = cat1->rightChild;
	
	    // 检查lastpos集，优先匹配 ab
	    auto& or1Set = dfa.m_lastpos.find(&or1)->second;
	    EXPECT_EQ(or1Set.size(), 2);
	    EXPECT_TRUE(or1Set.contains(&a));
	    EXPECT_TRUE(or1Set.contains(&c));
	
	    auto& cat1Set = dfa.m_lastpos.find(&cat1)->second;
	    EXPECT_EQ(cat1Set.size(), 1);
	    EXPECT_TRUE(cat1Set.contains(&d));
	}
	{
		SyntaxTree tree;
	    auto treeRet = tree.parse_regex("c(ba*)*");
	    ASSERT_TRUE(treeRet);
	    DFA dfa {};
	    dfa.create_graph(std::move(tree));
		auto& root = dfa.m_tree.m_root;
	    auto& cat1 = root->leftChild;
		auto& c = cat1->leftChild;
		auto& star1 = cat1->rightChild;
		auto& cat2 = star1->leftChild;
		auto& b = cat2->leftChild;
		auto& star2 = cat2->rightChild;
		auto& a = star2->leftChild;

		auto& star2Set = dfa.m_lastpos.find(&star2)->second;
		ASSERT_EQ(star2Set.size(), 1);
		ASSERT_TRUE(star2Set.contains(&a));

		auto& cat2Set = dfa.m_lastpos.find(&cat2)->second;
		EXPECT_EQ(cat2Set.size(), 2);
		EXPECT_TRUE(cat2Set.contains(&a));
		EXPECT_TRUE(cat2Set.contains(&b));

		auto& star1Set = dfa.m_lastpos.find(&star1)->second;
		EXPECT_EQ(star1Set.size(), 1);
		EXPECT_TRUE(cat2Set.contains(&b));

		//这里的cat1是使用star1的firstpos, 不是lastpos
		auto& cat1Set = dfa.m_lastpos.find(&cat1)->second;
		EXPECT_EQ(cat1Set.size(), 2);
		EXPECT_TRUE(cat1Set.contains(&c));
		EXPECT_TRUE(cat1Set.contains(&b));
		EXPECT_FALSE(cat1Set.contains(&a));
	}
}

TEST_F(TestDFA, test_followpos)
{
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("a");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		auto& root = dfa.m_tree.m_root;
		//没有被赋值的leaves节点就不存在于followpos中
		EXPECT_EQ(dfa.m_followpos.size(), 1);
		auto& a = root->leftChild;
		auto& end = root->rightChild;
		auto& setA = dfa.m_followpos.find(&a)->second;
		EXPECT_EQ(setA.size(), 1);
		EXPECT_TRUE(setA.contains(&end));
	}
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("ab");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		ASSERT_EQ(dfa.m_followpos.size(), 2);
		auto& root = dfa.m_tree.m_root;
		auto& end = root->rightChild;
		auto& cat1 = root->leftChild;
		auto& a = cat1->leftChild;
		auto& b = cat1->rightChild;
		auto& aSet = dfa.m_followpos.find(&a)->second;
		EXPECT_EQ(aSet.size(), 1);
		EXPECT_TRUE(aSet.contains(&b));
		auto& bSet = dfa.m_followpos.find(&b)->second;
		EXPECT_EQ(bSet.size(), 1);
		EXPECT_TRUE(bSet.contains(&end));
	}
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("a*");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		ASSERT_EQ(dfa.m_followpos.size(), 1);
		auto& root = dfa.m_tree.m_root;
		auto& end = root->rightChild;
		auto& a = root->leftChild->leftChild;
		auto& aSet = dfa.m_followpos.find(&a)->second;
		EXPECT_EQ(aSet.size(), 2);
		EXPECT_TRUE(aSet.contains(&a));
		EXPECT_TRUE(aSet.contains(&end));
	}
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("(a|b)*");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		auto& root = dfa.m_tree.m_root;
		auto& end = root->rightChild;
		ASSERT_EQ(dfa.m_followpos.size(), 2);
		auto& or1 = root->leftChild->leftChild;
		auto& a = or1->leftChild;
		auto& b = or1->rightChild;
		
		auto& aSet = dfa.m_followpos.find(&a)->second;
		EXPECT_EQ(aSet.size(), 3);
		EXPECT_TRUE(aSet.contains(&a));
		EXPECT_TRUE(aSet.contains(&b));
		EXPECT_TRUE(aSet.contains(&end));
		auto& bSet = dfa.m_followpos.find(&b)->second;
		EXPECT_EQ(aSet, bSet);
	}
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("(a|b)*abb");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		auto& root = dfa.m_tree.m_root;
		auto& end = root->rightChild;		//6
		EXPECT_EQ(dfa.m_followpos.size(), 5);
		auto& cat1 = root->leftChild;
		auto& cat2 = cat1->leftChild;
		auto& b5 = cat1->rightChild;
		auto& cat3 = cat2->leftChild;
		auto& b4 = cat2->rightChild;
		auto& star1 = cat3->leftChild;
		auto& a3 = cat3->rightChild;
		auto& or1 = star1->leftChild;
		auto& a1 = or1->leftChild;
		auto& b2 = or1->rightChild;
		
		auto& a1Set = dfa.m_followpos.find(&a1)->second;
		auto& b2Set = dfa.m_followpos.find(&b2)->second;
		EXPECT_EQ(a1Set.size(), 3);
		EXPECT_TRUE(a1Set.contains(&a1));
		EXPECT_TRUE(a1Set.contains(&b2));
		EXPECT_TRUE(a1Set.contains(&a3));
		EXPECT_EQ(b2Set, a1Set);

		auto& a3Set = dfa.m_followpos.find(&a3)->second;
		auto& b4Set = dfa.m_followpos.find(&b4)->second;
		auto& b5Set = dfa.m_followpos.find(&b5)->second;
		EXPECT_EQ(a3Set.size(), 1);
		EXPECT_EQ(b4Set.size(), 1);
		EXPECT_EQ(b5Set.size(), 1);
		EXPECT_TRUE(a3Set.contains(&b4));
		EXPECT_TRUE(b4Set.contains(&b5));
		EXPECT_TRUE(b5Set.contains(&end));
	}
}

TEST_F(TestDFA, test_construct_graph)
{
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("a");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_vertexTable.size(), 1);
	}
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("ab");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		//EXPECT_EQ(dfa.m_vertexTable.size(), 2);
	}
	{
		SyntaxTree tree;
		auto treeRet = tree.parse_regex("(a|b)*");
		ASSERT_TRUE(treeRet);
		DFA dfa {};
		dfa.create_graph(std::move(tree));
		EXPECT_EQ(dfa.m_vertexTable.size(), 1);
	}
}

}	//namespace simple_regex
