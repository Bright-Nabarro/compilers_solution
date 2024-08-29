#include <print>
#include <tl/expected.hpp>
#include <string_view>
#include <iostream>

std::string inputExpr;
size_t lookhead;

tl::expected<bool, std::string> match(bool exp, std::string_view sv = "")
{
    [[unlikely]] if (lookhead >= inputExpr.size())
        throw std::logic_error { "Unhandled lookhead reached the end" };
    
    if (exp)
    {
        if (++lookhead == inputExpr.size())
            return true;
        return false;
    }

    return tl::unexpected { sv.data() };
}

tl::expected<bool, std::string> S()
{
    // ( S ) S R
    if (auto ret = match(inputExpr[lookhead] == '('); ret)
    {
        ret = S();
        if (!ret)
            return ret;
        if (*ret)
            return tl::unexpected { "表达式中缺少`)`" };
        
        ret = match(inputExpr[lookhead] == ')', 
            std::format("表达式[{}]: {}不匹配`)`", lookhead, inputExpr[lookhead]));
        if (!ret)
            return ret;
        if (*ret)
            return true;

        ret = S();
        if (!ret)
            return ret;
        if (*ret)
            return true;

        ret = S();
        if (!ret)
            return ret;
        if (*ret)
            return true;
    }

    // ε
    return false;
}

int main()
{
    lookhead = 0;
    std::cin >> inputExpr;
    auto ret = S();
    if (ret)
    {
        if (!*ret)
            std::println("syntax error behind input string [{}] {}",
                    lookhead, inputExpr[lookhead]);
        else
            std::println("success");
    }
    else
    {
        std::println("syntax error: {}", ret.error());
    }
}

