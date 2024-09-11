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
}

}	//namespace simple_regex
