#include <gtest/gtest.h>
#include <sstream>
#include <print>
#include "parse.hpp"

using namespace simple_regex;

DFA dfaFactory(std::string_view sv)
{
	SyntaxTree tree;
	auto treeRet = tree.parse_regex(sv);
	assert(treeRet);
	DFA dfa;
	dfa.create_graph(std::move(tree));
	return dfa;
}

auto parse_and_get(auto& parse, auto& ssm, std::string_view sv)
{
	parse.clear_result();
	ssm.clear();
	ssm << sv.data();
	auto parseStreamRet = parse.parse_stream(ssm);
	assert(parseStreamRet);
	return parse.get_parse_string_list();

}

TEST(TestParse, test_parse_stream_normal)
{
	{
		std::stringstream ssm;
		Parse parse { dfaFactory("a") };
		ssm<<"a";
		auto parseStreamRet = parse.parse_stream(ssm);
		ASSERT_TRUE(parseStreamRet);
		auto parseStringList = parse.get_parse_string_list();
		EXPECT_EQ(parseStringList.size(), 1);
		EXPECT_EQ(parseStringList[0].context, "a");
		EXPECT_EQ(parseStringList[0].begPos, 0);

		ssm.clear();
		ssm<<"aa";
		parse.clear_result();
		parseStreamRet = parse.parse_stream(ssm);
		ASSERT_TRUE(parseStreamRet);
		parseStringList = parse.get_parse_string_list();
		EXPECT_EQ(parseStringList.size(), 2);
		EXPECT_EQ(parseStringList[0].context, "a");
		EXPECT_EQ(parseStringList[0].begPos, 0);
		EXPECT_EQ(parseStringList[1].context, "a");
		EXPECT_EQ(parseStringList[1].begPos, 1);
		ssm.clear();
		ssm<<"aaa";
		parse.clear_result();
		parseStreamRet = parse.parse_stream(ssm);
		ASSERT_TRUE(parseStreamRet);
		parseStringList = parse.get_parse_string_list();
		EXPECT_EQ(parseStringList.size(), 3);
		EXPECT_EQ(parseStringList[0].context, "a");
		EXPECT_EQ(parseStringList[0].begPos, 0);
		EXPECT_EQ(parseStringList[1].context, "a");
		EXPECT_EQ(parseStringList[1].begPos, 1);
		EXPECT_EQ(parseStringList[2].context, "a");
		EXPECT_EQ(parseStringList[2].begPos, 2);
		auto ret = parse_and_get(parse, ssm, "1a2a3a4a5a6a7a8a9a10a");
		EXPECT_EQ(ret.size(), 10);
		for (size_t i = 0; i < 10; i++)
		{
			EXPECT_EQ(ret[i].context, "a");
		}
	}

	//下面使用chatgpt+手工修改，注释和EXPECT可能存在对应不上的情况
	{
		std::stringstream ssm;
		Parse parse { dfaFactory("a*") };
	
		// Test case 1: Simple input with a single "a"
		auto ret = parse_and_get(parse, ssm, "a");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "a");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 2: Multiple "a" characters in a row
		ret = parse_and_get(parse, ssm, "aaa");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "aaa");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 3: Mixed "a" and other characters
		ret = parse_and_get(parse, ssm, "abaabaaab");
		EXPECT_EQ(ret.size(), 3);
		EXPECT_EQ(ret[0].context, "a");
		EXPECT_EQ(ret[0].begPos, 0);
		EXPECT_EQ(ret[1].context, "aa");
		EXPECT_EQ(ret[1].begPos, 2);
		EXPECT_EQ(ret[2].context, "aaa");
		EXPECT_EQ(ret[2].begPos, 5);
	
		// Test case 4: Input string with alternating "a" and other characters
		ret = parse_and_get(parse, ssm, "ababa");
		EXPECT_EQ(ret.size(), 3);
		EXPECT_EQ(ret[0].context, "a");
		EXPECT_EQ(ret[0].begPos, 0);
		EXPECT_EQ(ret[1].context, "a");
		EXPECT_EQ(ret[1].begPos, 2);
		EXPECT_EQ(ret[2].context, "a");
		EXPECT_EQ(ret[2].begPos, 4);
	
		// Test case 5: Input string starts and ends with "a"
		ret = parse_and_get(parse, ssm, "aaabbbaa");
		EXPECT_EQ(ret.size(), 2);
		EXPECT_EQ(ret[0].context, "aaa");
		EXPECT_EQ(ret[0].begPos, 0);
		EXPECT_EQ(ret[1].context, "aa");
		EXPECT_EQ(ret[1].begPos, 6);
	
		// Test case 6: Input string with consecutive "a"s separated by other characters
		ret = parse_and_get(parse, ssm, "aabbbaaaab");
		EXPECT_EQ(ret.size(), 2);
		EXPECT_EQ(ret[0].context, "aa");
		EXPECT_EQ(ret[0].begPos, 0);
		EXPECT_EQ(ret[1].context, "aaaa");
		EXPECT_EQ(ret[1].begPos, 5);
	
		// Test case 7: Input string with a single "a" at the end
		ret = parse_and_get(parse, ssm, "bbba");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "a");
		EXPECT_EQ(ret[0].begPos, 3);
	
		// Test case 8: Input string with multiple "a"s followed by non-"a" characters
		ret = parse_and_get(parse, ssm, "aaaabc");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "aaaa");
		EXPECT_EQ(ret[0].begPos, 0);
	}
	{
		std::stringstream ssm;
		Parse parse { dfaFactory("abc") };
	
		// Test case 1: Exact match "abc"
		auto ret = parse_and_get(parse, ssm, "abc");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "abc");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 2: Input with "abc" at the beginning
		ret = parse_and_get(parse, ssm, "abcxyz");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "abc");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 3: Input with "abc" in the middle
		ret = parse_and_get(parse, ssm, "xyzabcxyz");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "abc");
		EXPECT_EQ(ret[0].begPos, 3);
	
		// Test case 4: Multiple occurrences of "abc"
		ret = parse_and_get(parse, ssm, "abcabc");
		EXPECT_EQ(ret.size(), 2);
		EXPECT_EQ(ret[0].context, "abc");
		EXPECT_EQ(ret[0].begPos, 0);
		EXPECT_EQ(ret[1].context, "abc");
		EXPECT_EQ(ret[1].begPos, 3);
	
		// Test case 5: Input without "abc"
		ret = parse_and_get(parse, ssm, "xyz");
		EXPECT_EQ(ret.size(), 0);  // No matches found
	}
	{
		std::stringstream ssm;
		Parse parse { dfaFactory("(a|b)c") };
	
		// Test case 1: Exact match "ac"
		auto ret = parse_and_get(parse, ssm, "ac");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "ac");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 2: Exact match "bc"
		ret = parse_and_get(parse, ssm, "bc");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "bc");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 3: Input starts with "ac"
		ret = parse_and_get(parse, ssm, "acxyz");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "ac");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 4: Input starts with "bc"
		ret = parse_and_get(parse, ssm, "bcxyz");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "bc");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 5: Input contains "ac" in the middle
		ret = parse_and_get(parse, ssm, "xyzacxyz");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "ac");
		EXPECT_EQ(ret[0].begPos, 3);
	
		// Test case 6: Input contains "bc" in the middle
		ret = parse_and_get(parse, ssm, "xyzbcxyz");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "bc");
		EXPECT_EQ(ret[0].begPos, 3);
	
		// Test case 7: Multiple "ac" and "bc" occurrences
		ret = parse_and_get(parse, ssm, "acbcac");
		EXPECT_EQ(ret.size(), 3);
		EXPECT_EQ(ret[0].context, "ac");
		EXPECT_EQ(ret[0].begPos, 0);
		EXPECT_EQ(ret[1].context, "bc");
		EXPECT_EQ(ret[1].begPos, 2);
		EXPECT_EQ(ret[2].context, "ac");
		EXPECT_EQ(ret[2].begPos, 4);
	
		// Test case 8: Input with extra characters but no matches
		ret = parse_and_get(parse, ssm, "xyz");
		EXPECT_EQ(ret.size(), 0);  // No matches found
	
		// Test case 9: Overlapping patterns with "ac" and "bc"
		ret = parse_and_get(parse, ssm, "abccabc");
		EXPECT_EQ(ret.size(), 2);
		EXPECT_EQ(ret[0].context, "bc");
		EXPECT_EQ(ret[0].begPos, 1);
		EXPECT_EQ(ret[1].context, "bc");
		EXPECT_EQ(ret[1].begPos, 5);
	
		// Test case 10: Input with partial matches that do not satisfy the full pattern
		ret = parse_and_get(parse, ssm, "aabbbcc");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "bc");
		EXPECT_EQ(ret[0].begPos, 4);
	}
	{
		std::stringstream ssm;
		Parse parse { dfaFactory("(a|b)*") };
	
		// Test case 1: Input with a single "a"
		auto ret = parse_and_get(parse, ssm, "a");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "a");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 2: Input with a single "b"
		ret = parse_and_get(parse, ssm, "b");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "b");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 3: Input with alternating "a" and "b"
		ret = parse_and_get(parse, ssm, "abab");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "abab");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 4: Input with all "a"s
		ret = parse_and_get(parse, ssm, "aaaa");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "aaaa");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 5: Input with all "b"s
		ret = parse_and_get(parse, ssm, "bbbb");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "bbbb");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 6: Input with mixed "a" and "b", more "a"s than "b"s
		ret = parse_and_get(parse, ssm, "aaabbaa");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "aaabbaa");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 7: Input with "a" and "b" and extra characters
		ret = parse_and_get(parse, ssm, "ababxyz");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "abab");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 8: Input starts with extra characters, followed by "a" and "b"
		ret = parse_and_get(parse, ssm, "xyzabab");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "abab");
		EXPECT_EQ(ret[0].begPos, 3);
	
		// Test case 9: Empty string (should match empty string as it's part of the regex)
		//ret = parse_and_get(parse, ssm, "");
		//EXPECT_EQ(ret.size(), 1);
		//EXPECT_EQ(ret[0].context, "");
		//EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 10: Input with partial matches that do not satisfy the full pattern
		ret = parse_and_get(parse, ssm, "aabbcc");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "aabb");
		EXPECT_EQ(ret[0].begPos, 0);
	}

	{
		std::stringstream ssm;
		Parse parse { dfaFactory("(a|b|c|d)*abcd") };
	
		// Test case 1: Exact match "abcd"
		auto ret = parse_and_get(parse, ssm, "abcd");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "abcd");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 2: Input starts with valid pattern followed by "abcd"
		ret = parse_and_get(parse, ssm, "bbccdabcdd");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "bbccdabcd");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 3: Input contains random pattern but ends with "abcd"
		ret = parse_and_get(parse, ssm, "ddaaabcd");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "ddaaabcd");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 4: Multiple valid patterns followed by "abcd"
		ret = parse_and_get(parse, ssm, "aacbddbcbabcd");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "aacbddbcbabcd");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 5: Input starts with extra characters and ends with valid "abcd" pattern
		ret = parse_and_get(parse, ssm, "xyzabcd");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "abcd");
        EXPECT_EQ(ret[0].begPos, 3);
    }

    {
        std::stringstream ssm;
        Parse parse{dfaFactory("(a|b)*abb(a|b)*")};

        // Test case 1: Exact match "abb"
        auto ret = parse_and_get(parse, ssm, "abb");
        EXPECT_EQ(ret.size(), 1);
        EXPECT_EQ(ret[0].context, "abb");
        EXPECT_EQ(ret[0].begPos, 0);

        // Test case 2: Input starts and ends with "a" or "b" around "abb"
        ret = parse_and_get(parse, ssm, "aabbba");
        EXPECT_EQ(ret.size(), 1);
        EXPECT_EQ(ret[0].context, "aabbba");
        EXPECT_EQ(ret[0].begPos, 0);

        // Test case 3: Input with only "a" or "b" before and after "abb"
        ret = parse_and_get(parse, ssm, "bbbbabbbaa");
        EXPECT_EQ(ret.size(), 1);
        EXPECT_EQ(ret[0].context, "bbbbabbbaa");
        EXPECT_EQ(ret[0].begPos, 0);

        // Test case 4: Multiple occurrences of valid pattern
        ret = parse_and_get(parse, ssm, "aabbabbabb");
        EXPECT_EQ(ret.size(), 1);
        EXPECT_EQ(ret[0].context, "aabbabbabb");
        EXPECT_EQ(ret[0].begPos, 0);

        // Test case 5: Extra characters before and after the valid pattern
        ret = parse_and_get(parse, ssm, "xxaabbabbxx");
        EXPECT_EQ(ret.size(), 1);
        EXPECT_EQ(ret[0].context, "aabbabb");
        EXPECT_EQ(ret[0].begPos, 2);
    }
	{
		std::stringstream ssm;
		Parse parse { dfaFactory("((a|b)*c)*d(a|b)*") };
	
		// Test case 1: Exact match with single "d" and mixed "a", "b", and "c"
		auto ret = parse_and_get(parse, ssm, "abcd");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "abcd");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 2: Multiple groups of (a|b)*c followed by "d" and (a|b)*
		ret = parse_and_get(parse, ssm, "aabcbcadbbb");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "aabcbcadbbb");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 3: Complex nested structure with multiple c's and trailing a's and b's
		ret = parse_and_get(parse, ssm, "bccbccbccdba");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "bccbccbccdba");
		EXPECT_EQ(ret[0].begPos, 0);
	
		// Test case 4: Input with extra characters before and after valid match
		ret = parse_and_get(parse, ssm, "xxabcbcadbaxx");
		EXPECT_EQ(ret.size(), 1);
		EXPECT_EQ(ret[0].context, "abcbcadba");
		EXPECT_EQ(ret[0].begPos, 2);
	}

    //=========END==========
}
