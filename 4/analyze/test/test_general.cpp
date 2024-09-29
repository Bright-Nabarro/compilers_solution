#include <fstream>
#include <print>
#include <gtest/gtest.h>

#include "grammar.hpp"

using namespace analyze;

TEST(TestGrammar, test_start_symbol)
{
    Grammar grammar;

    // 1. 测试在未设置 start_symbol 时调用 get_start
    {
        auto result = grammar.get_start();
        EXPECT_FALSE(result.has_value());  // 应该返回失败的 expected
        EXPECT_EQ(result.error(), "start symbol does not spectialized");
    }

    // 2. 测试设置 start_symbol 为终结符时的异常
    {
        Symbol terminal_symbol(Symbol::terminal, "terminal_symbol");
        EXPECT_THROW(grammar.set_start(terminal_symbol), std::logic_error);
    }

    // 3. 测试设置 start_symbol 为合法的非终结符
    {
        Symbol no_terminal_symbol(Symbol::no_terminal, "non_terminal_symbol");
        EXPECT_NO_THROW(grammar.set_start(no_terminal_symbol));

        // 验证 get_start 返回正确的符号
        auto result = grammar.get_start();
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), no_terminal_symbol);  // 符号应匹配
    }

    // 4. 测试替换 start_symbol
    {
        Symbol new_no_terminal_symbol(Symbol::no_terminal, "new_non_terminal_symbol");
        EXPECT_NO_THROW(grammar.set_start(new_no_terminal_symbol));

        // 检查新的 start_symbol 是否替换了旧的
        auto result = grammar.get_start();
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), new_no_terminal_symbol);  // 应该是新的符号
    }

    // 5. 测试通过布尔值构造符号（非终结符）
    {
        Symbol bool_constructed_symbol(false, "bool_constructed_symbol");
        EXPECT_NO_THROW(grammar.set_start(bool_constructed_symbol));

        // 验证 get_start 返回通过布尔值构造的符号
        auto result = grammar.get_start();
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), bool_constructed_symbol);  // 符号应匹配
    }
//end_function
}

TEST(TestGrammar, test_add_rules)
{
	Grammar grammar;

    // 创建终结符和非终结符
    Symbol terminal_symbol(Symbol::terminal, "a");
    Symbol non_terminal_symbol(Symbol::no_terminal, "A");

    // 添加规则，右部为终结符的向量
    std::vector<Symbol> rule_right = {terminal_symbol};

    // 调用add_rules，确保返回true，表明添加成功
    ASSERT_TRUE(grammar.add_rule(non_terminal_symbol, rule_right));

    // 再次插入相同规则，确保返回false，表明没有重复插入
    ASSERT_FALSE(grammar.add_rule(non_terminal_symbol, rule_right));

    // 插入新的规则，右部包含多个符号
    std::vector<Symbol> new_rule_right = {non_terminal_symbol, terminal_symbol};
    ASSERT_TRUE(grammar.add_rule(non_terminal_symbol, new_rule_right));
}

TEST(TestYamlParser, test_parse)
{
	YamlParser parser;
	{
		std::ifstream in_file { "./test/test_yaml/1.yaml" };
		ASSERT_TRUE(in_file);
		auto ret = parser.parse(in_file);
		ASSERT_TRUE(ret);
		auto& grammar = *ret;
		auto check_ret = grammar.validate_noterminals();
		ASSERT_TRUE(check_ret);
		auto start_ret = grammar.get_start();
		ASSERT_TRUE(start_ret);
		EXPECT_EQ(start_ret->type(), Symbol::no_terminal);
		EXPECT_EQ(start_ret->symbol_string(), "S");
		auto set_ret = grammar.find(*grammar.get_start());
		ASSERT_TRUE(set_ret);
		auto& set = set_ret->get();
		ASSERT_EQ(set.size(), 1);
		auto& vec = *set.begin();
		EXPECT_EQ(vec.front().symbol_string(), "a");
	}

	{
		std::ifstream in_file { "./test/test_yaml/2.yaml" };
		ASSERT_TRUE(in_file);
		auto ret = parser.parse(in_file);
		ASSERT_TRUE(ret);
		auto& grammar = *ret;
		auto check_ret = grammar.validate_noterminals();
		ASSERT_TRUE(check_ret);
		auto start_ret = grammar.get_start();
		ASSERT_TRUE(start_ret);
		EXPECT_EQ(start_ret->type(), Symbol::no_terminal);
		EXPECT_EQ(start_ret->symbol_string(), "S");
		auto set_ret = grammar.find(*grammar.get_start());
		ASSERT_TRUE(set_ret);
		auto& set = set_ret->get();
		ASSERT_EQ(set.size(), 2);
		for (const auto& vec : set)
		{
			if (vec.size() == 2)
			{
				EXPECT_EQ(vec.front().symbol_string(), "a");
				EXPECT_EQ(vec.front().type(), Symbol::terminal);
				EXPECT_EQ(vec.back().symbol_string(), "S");
				EXPECT_EQ(vec.back().type(), Symbol::no_terminal);
			}
			else if(vec.size() == 1)
			{
				EXPECT_EQ(vec.front().symbol_string(), "b");
				EXPECT_EQ(vec.front().type(), Symbol::terminal);
			}
			else
			{
				FAIL() << "vec.size() expected";
			}
		}
	}

	{
    	std::ifstream in_file { "./test/test_yaml/3.yaml" };  // 假设新的文法文件为 3.yaml
    	ASSERT_TRUE(in_file);
    	auto ret = parser.parse(in_file);
    	ASSERT_TRUE(ret);
    	auto& grammar = *ret;

    	// 验证非终结符是否都出现
    	auto check_ret = grammar.validate_noterminals();
    	ASSERT_TRUE(check_ret);

    	// 检查开始符号
    	auto start_ret = grammar.get_start();
    	ASSERT_TRUE(start_ret);
    	EXPECT_EQ(start_ret->type(), Symbol::no_terminal);
    	EXPECT_EQ(start_ret->symbol_string(), "S");

    	// 检查 S -> AB 规则
    	auto set_ret = grammar.find(*grammar.get_start());
    	ASSERT_TRUE(set_ret);
    	auto& set = set_ret->get();
    	ASSERT_EQ(set.size(), 1);
    	auto& vec = *set.begin();
    	EXPECT_EQ(vec.size(), 2);
    	EXPECT_EQ(vec[0].symbol_string(), "A");
    	EXPECT_EQ(vec[0].type(), Symbol::no_terminal);
    	EXPECT_EQ(vec[1].symbol_string(), "B");
    	EXPECT_EQ(vec[1].type(), Symbol::no_terminal);

    	// 查找 A 的规则，A -> a
    	auto set_ret_A = grammar.find(Symbol(Symbol::no_terminal, "A"));
    	ASSERT_TRUE(set_ret_A);
    	auto& set_A = set_ret_A->get();
    	ASSERT_EQ(set_A.size(), 1);
    	auto& vec_A = *set_A.begin();
    	EXPECT_EQ(vec_A.size(), 1);
    	EXPECT_EQ(vec_A[0].symbol_string(), "a");
    	EXPECT_EQ(vec_A[0].type(), Symbol::terminal);

    	// 查找 B 的规则，B -> b
    	auto set_ret_B = grammar.find(Symbol(Symbol::no_terminal, "B"));
    	ASSERT_TRUE(set_ret_B);
    	auto& set_B = set_ret_B->get();
    	ASSERT_EQ(set_B.size(), 1);
    	auto& vec_B = *set_B.begin();
    	EXPECT_EQ(vec_B.size(), 1);
    	EXPECT_EQ(vec_B[0].symbol_string(), "b");
    	EXPECT_EQ(vec_B[0].type(), Symbol::terminal);
	}
	{
	    std::ifstream in_file { "./test/test_yaml/4.yaml" };  // 假设新的文法文件为 4.yaml
	    ASSERT_TRUE(in_file);
	    auto ret = parser.parse(in_file);
	    ASSERT_TRUE(ret);
	    auto& grammar = *ret;
	
	    // 验证非终结符是否都出现
	    auto check_ret = grammar.validate_noterminals();
	    ASSERT_TRUE(check_ret);
	
	    // 检查开始符号
	    auto start_ret = grammar.get_start();
	    ASSERT_TRUE(start_ret);
	    EXPECT_EQ(start_ret->type(), Symbol::no_terminal);
	    EXPECT_EQ(start_ret->symbol_string(), "S");
	
	    // 查找 S 的规则
	    auto set_ret = grammar.find(*grammar.get_start());
	    ASSERT_TRUE(set_ret);
	    auto& set = set_ret->get();
	    ASSERT_EQ(set.size(), 2);  // S 有两个规则
	
	    // 检查 S -> aS 和 S -> b 规则
	    bool found_aS = false;
	    bool found_b = false;
	    for (const auto& vec : set)
	    {
	        if (vec.size() == 2)
	        {
	            // S -> aS
	            EXPECT_EQ(vec[0].symbol_string(), "a");
	            EXPECT_EQ(vec[0].type(), Symbol::terminal);
	            EXPECT_EQ(vec[1].symbol_string(), "S");
	            EXPECT_EQ(vec[1].type(), Symbol::no_terminal);
	            found_aS = true;
	        }
	        else if (vec.size() == 1)
	        {
	            // S -> b
	            EXPECT_EQ(vec[0].symbol_string(), "b");
	            EXPECT_EQ(vec[0].type(), Symbol::terminal);
	            found_b = true;
	        }
	        else
	        {
	            FAIL() << "Unexpected rule size for S";
	        }
	    }
	
	    // 检查是否找到了两个规则
	    EXPECT_TRUE(found_aS);
	    EXPECT_TRUE(found_b);
	}
	{
	    std::ifstream in_file { "./test/test_yaml/5.yaml" };  // 假设文法文件为 5.yaml
	    ASSERT_TRUE(in_file);
	    auto ret = parser.parse(in_file);
	    ASSERT_TRUE(ret);
	    auto& grammar = *ret;
	
	    // 验证非终结符是否都出现
	    auto check_ret = grammar.validate_noterminals();
	    ASSERT_TRUE(check_ret);
	
	    // 检查开始符号 E
	    auto start_ret = grammar.get_start();
	    ASSERT_TRUE(start_ret);
	    EXPECT_EQ(start_ret->type(), Symbol::no_terminal);
	    EXPECT_EQ(start_ret->symbol_string(), "E");
	
	    // 检查 E 的规则: E -> E + T, E -> T
	    auto set_ret_E = grammar.find(*grammar.get_start());
	    ASSERT_TRUE(set_ret_E);
	    auto& set_E = set_ret_E->get();
	    ASSERT_EQ(set_E.size(), 2);  // E 有两个规则
	
	    bool found_E_plus_T = false;
	    bool found_E_to_T = false;
	    for (const auto& vec : set_E)
	    {
	        if (vec.size() == 3 && vec[1].symbol_string() == "+")
	        {
	            // E -> E + T
	            EXPECT_EQ(vec[0].symbol_string(), "E");
	            EXPECT_EQ(vec[0].type(), Symbol::no_terminal);
	            EXPECT_EQ(vec[1].symbol_string(), "+");
	            EXPECT_EQ(vec[1].type(), Symbol::terminal);
	            EXPECT_EQ(vec[2].symbol_string(), "T");
	            EXPECT_EQ(vec[2].type(), Symbol::no_terminal);
	            found_E_plus_T = true;
	        }
	        else if (vec.size() == 1)
	        {
	            // E -> T
	            EXPECT_EQ(vec[0].symbol_string(), "T");
	            EXPECT_EQ(vec[0].type(), Symbol::no_terminal);
	            found_E_to_T = true;
	        }
	    }
	    EXPECT_TRUE(found_E_plus_T);
	    EXPECT_TRUE(found_E_to_T);
	
	    // 检查 T 的规则: T -> T * F, T -> F
	    auto set_ret_T = grammar.find(Symbol(Symbol::no_terminal, "T"));
	    ASSERT_TRUE(set_ret_T);
	    auto& set_T = set_ret_T->get();
	    ASSERT_EQ(set_T.size(), 2);  // T 有两个规则
	
	    bool found_T_times_F = false;
	    bool found_T_to_F = false;
	    for (const auto& vec : set_T)
	    {
	        if (vec.size() == 3 && vec[1].symbol_string() == "*")
	        {
	            // T -> T * F
	            EXPECT_EQ(vec[0].symbol_string(), "T");
	            EXPECT_EQ(vec[0].type(), Symbol::no_terminal);
	            EXPECT_EQ(vec[1].symbol_string(), "*");
	            EXPECT_EQ(vec[1].type(), Symbol::terminal);
	            EXPECT_EQ(vec[2].symbol_string(), "F");
	            EXPECT_EQ(vec[2].type(), Symbol::no_terminal);
	            found_T_times_F = true;
	        }
	        else if (vec.size() == 1)
	        {
	            // T -> F
	            EXPECT_EQ(vec[0].symbol_string(), "F");
	            EXPECT_EQ(vec[0].type(), Symbol::no_terminal);
	            found_T_to_F = true;
	        }
	    }
	    EXPECT_TRUE(found_T_times_F);
	    EXPECT_TRUE(found_T_to_F);
	
	    // 检查 F 的规则: F -> (E), F -> a
	    auto set_ret_F = grammar.find(Symbol(Symbol::no_terminal, "F"));
	    ASSERT_TRUE(set_ret_F);
	    auto& set_F = set_ret_F->get();
	    ASSERT_EQ(set_F.size(), 2);  // F 有两个规则
	
	    bool found_F_paren_E = false;
	    bool found_F_to_a = false;
	    for (const auto& vec : set_F)
	    {
	        if (vec.size() == 3 && vec[0].symbol_string() == "(")
	        {
	            // F -> (E)
	            EXPECT_EQ(vec[0].symbol_string(), "(");
	            EXPECT_EQ(vec[0].type(), Symbol::terminal);
	            EXPECT_EQ(vec[1].symbol_string(), "E");
	            EXPECT_EQ(vec[1].type(), Symbol::no_terminal);
	            EXPECT_EQ(vec[2].symbol_string(), ")");
	            EXPECT_EQ(vec[2].type(), Symbol::terminal);
	            found_F_paren_E = true;
	        }
	        else if (vec.size() == 1)
	        {
	            // F -> a
	            EXPECT_EQ(vec[0].symbol_string(), "a");
	            EXPECT_EQ(vec[0].type(), Symbol::terminal);
	            found_F_to_a = true;
	        }
	    }
	    EXPECT_TRUE(found_F_paren_E);
	    EXPECT_TRUE(found_F_to_a);
	}
	{
    	std::ifstream in_file { "./test/test_yaml/6.yaml" };  // 假设文法文件为 6.yaml
    	ASSERT_TRUE(in_file);
    	auto ret = parser.parse(in_file);
    	ASSERT_TRUE(ret);
    	auto& grammar = *ret;

    	// 验证非终结符是否都出现
    	auto check_ret = grammar.validate_noterminals();
    	ASSERT_TRUE(check_ret);

    	// 检查开始符号 S
    	auto start_ret = grammar.get_start();
    	ASSERT_TRUE(start_ret);
    	EXPECT_EQ(start_ret->type(), Symbol::no_terminal);
    	EXPECT_EQ(start_ret->symbol_string(), "S");

    	// 查找 S 的规则: S -> A, S -> B
    	auto set_ret_S = grammar.find(*grammar.get_start());
    	ASSERT_TRUE(set_ret_S);
    	auto& set_S = set_ret_S->get();
    	ASSERT_EQ(set_S.size(), 2);  // S 有两条规则

    	bool found_A = false;
    	bool found_B = false;
    	for (const auto& vec : set_S)
    	{
    	    if (vec.size() == 1 && vec[0].symbol_string() == "A")
    	    {
    	        found_A = true;
    	    }
    	    else if (vec.size() == 1 && vec[0].symbol_string() == "B")
    	    {
    	        found_B = true;
    	    }
    	    else
    	    {
    	        FAIL() << "Unexpected rule for S";
    	    }
    	}
    	EXPECT_TRUE(found_A);
    	EXPECT_TRUE(found_B);

    	// 检查 A 的规则: A -> aA, A -> ε (空串)
    	auto set_ret_A = grammar.find(Symbol(Symbol::no_terminal, "A"));
    	ASSERT_TRUE(set_ret_A);
    	auto& set_A = set_ret_A->get();
    	ASSERT_EQ(set_A.size(), 2);  // A 有两条规则

    	bool found_aA = false;
    	bool found_epsilon_A = false;
    	for (const auto& vec : set_A)
    	{
    	    if (vec.size() == 2 && vec[0].symbol_string() == "a" && vec[1].symbol_string() == "A")
    	    {
    	        // A -> aA
    	        EXPECT_EQ(vec[0].type(), Symbol::terminal);
    	        EXPECT_EQ(vec[1].type(), Symbol::no_terminal);
    	        found_aA = true;
    	    }
    	    else if (vec.empty())
    	    {
    	        // A -> ε (空串)
    	        found_epsilon_A = true;
    	    }
    	    else
    	    {
    	        FAIL() << "Unexpected rule for A";
    	    }
    	}
    	EXPECT_TRUE(found_aA);
    	EXPECT_TRUE(found_epsilon_A);

    	// 检查 B 的规则: B -> bB, B -> ε (空串)
    	auto set_ret_B = grammar.find(Symbol(Symbol::no_terminal, "B"));
    	ASSERT_TRUE(set_ret_B);
    	auto& set_B = set_ret_B->get();
    	ASSERT_EQ(set_B.size(), 2);  // B 有两条规则

    	bool found_bB = false;
    	bool found_epsilon_B = false;
    	for (const auto& vec : set_B)
    	{
    	    if (vec.size() == 2 && vec[0].symbol_string() == "b" && vec[1].symbol_string() == "B")
    	    {
    	        // B -> bB
    	        EXPECT_EQ(vec[0].type(), Symbol::terminal);
    	        EXPECT_EQ(vec[1].type(), Symbol::no_terminal);
    	        found_bB = true;
    	    }
    	    else if (vec.empty())
    	    {
    	        // B -> ε (空串)
    	        found_epsilon_B = true;
    	    }
    	    else
    	    {
    	        FAIL() << "Unexpected rule for B";
    	    }
    	}
    	EXPECT_TRUE(found_bB);
    	EXPECT_TRUE(found_epsilon_B);
	}
//=====function end
}

TEST(TestYamlParser, test_output)
{
	YamlParser parser;
	auto initial_grammar = [&](std::string_view sv) -> Grammar
	{
		std::ifstream in_file {sv.data()};
		assert(in_file);
		auto ret = parser.parse(in_file);
		assert(ret);
		return std::move(*ret);
		
	};

	std::ofstream outfile { "output.md" };
	Grammar grammar;
	for (size_t i = 1; i <= 6; i++)
	{
		std::println(outfile, "\nrule{}", i);
		grammar = initial_grammar(std::format("./test/test_yaml/{}.yaml", i));
		grammar.display_latex(outfile, true);
	}
	
}

