#include <gtest/gtest.h>
#include <print>
#include "dfa.hpp"
using namespace simple_regex;

//解决针对表达式(x_1|x_2)x_3x_4*形式dfa终结符标记错误问题
TEST(End, test_end_early)
{
	SyntaxTree tree;
	auto parseRegexExpt = tree.parse_regex("(a|b)cd*");
	ASSERT_TRUE(parseRegexExpt);
	tree.display(std::cout);
	DFA dfa;
	dfa.create_graph(std::move(tree));
	dfa.display_graph(std::cout);
	std::println(stdout, "followpos:");
	dfa.display_followpos(std::cout);
}
