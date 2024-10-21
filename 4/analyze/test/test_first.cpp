#include <gtest/gtest.h>
#include <fstream>

#include "grammar_factory.hpp"
#include "first.hpp"
using namespace analyze;

TEST(FirstSet, test_first)
{
	YamlParser yaml_parser;
	GrammarFactory grammar_factory(yaml_parser);
	auto create_first_set = [&](std::string_view sv)
	{
		std::ifstream in(sv.data());
		assert(in);
		auto grammar_ret = grammar_factory.create_grammar(in);
		assert(grammar_ret);
		return FirstSet {grammar_ret.value()};
	};

	Symbol S {Symbol::no_terminal, "S"};
	Symbol a {Symbol::terminal, "a"};
	{
		auto first = create_first_set("./test/test_yaml/1.yaml");
		EXPECT_TRUE(first.in_first_set(S, a));
	}

	Symbol b {Symbol::terminal, "b"};
	{
		auto first = create_first_set("./test/test_yaml/2.yaml");
		EXPECT_TRUE(first.in_first_set(S, a));
		EXPECT_TRUE(first.in_first_set(S, b));
	}
	Symbol A {Symbol::no_terminal, "A"};
	Symbol B {Symbol::no_terminal, "B"};
	{
		auto first = create_first_set("./test/test_yaml/3.yaml");
		EXPECT_TRUE(first.in_first_set(S, a));
		EXPECT_FALSE(first.in_first_set(S, b));
		EXPECT_TRUE(first.in_first_set(A, a));
		EXPECT_TRUE(first.in_first_set(B, b));
	}
	{
		auto first = create_first_set("./test/test_yaml/4.yaml");
		EXPECT_TRUE(first.in_first_set(S, a));
		EXPECT_TRUE(first.in_first_set(S, b));
	}
	Symbol E {Symbol::no_terminal, "E"};
	Symbol T {Symbol::no_terminal, "T"};
	Symbol F {Symbol::no_terminal, "F"};
	Symbol lp {Symbol::terminal, "("};
	Symbol rp {Symbol::terminal, ")"};
	Symbol add {Symbol::terminal, "+"};
	Symbol mul {Symbol::terminal, "*"};
	{
		auto first = create_first_set("./test/test_yaml/5.yaml");
		EXPECT_TRUE(first.in_first_set(E, lp));
		EXPECT_TRUE(first.in_first_set(E, a));
		EXPECT_FALSE(first.in_first_set(E, add));
		EXPECT_FALSE(first.in_first_set(E, mul));
		EXPECT_FALSE(first.in_first_set(E, rp));
		EXPECT_TRUE(first.in_first_set(F, lp));
		EXPECT_TRUE(first.in_first_set(F, lp));
		EXPECT_TRUE(first.in_first_set(T, lp));
		EXPECT_TRUE(first.in_first_set(T, lp));
	}
	{
		auto first = create_first_set("./test/test_yaml/6.yaml");
		EXPECT_TRUE(first.in_first_set(S, Grammar::empty_symbol));
		EXPECT_TRUE(first.in_first_set(S, a));
		EXPECT_TRUE(first.in_first_set(S, b));
		EXPECT_TRUE(first.in_first_set(A, Grammar::empty_symbol));
		EXPECT_TRUE(first.in_first_set(A, a));
		EXPECT_TRUE(first.in_first_set(B, Grammar::empty_symbol));
		EXPECT_TRUE(first.in_first_set(B, b));
	}
	Symbol c {Symbol::terminal, "c"};
	{
		auto first = create_first_set("./test/test_yaml/7.yaml");
		EXPECT_TRUE(first.in_first_set(S, a));
		EXPECT_TRUE(first.in_first_set(S, b));
		EXPECT_TRUE(first.in_first_set(S, c));
		EXPECT_TRUE(first.in_first_set(S, Grammar::empty_symbol));
	}
	{
		auto first = create_first_set("./test/test_yaml/8.yaml");
		EXPECT_TRUE(first.in_first_set(S, a));
		EXPECT_TRUE(first.in_first_set(S, b));
		EXPECT_TRUE(first.in_first_set(S, c));
		EXPECT_TRUE(first.in_first_set(S, Grammar::empty_symbol));
	}
}

