#include <print>
#include <tl/expected.hpp>
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
    auto ret = match( inputExpr[lookhead] == '0');
    if (!ret)
        return ret;
    
    ret = S();
    if (ret && *ret)
        return tl::unexpected { std::format("表达式缺少1") };

    ret = match( inputExpr[lookhead] == '1',
            std::format("表达式[{}]: {}不匹配`1`", lookhead, inputExpr[lookhead]));
    return ret;
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

