#include <string>
#include <print>
#include <cctype>
#include <format>
#include <optional>
#include <iostream>
#include <variant>

std::string rowExpr;
size_t lookhead;

std::variant<bool, std::string> match(bool m, std::string_view errMsg)
{
    if (m)
    {
        lookhead++;
        if (lookhead == rowExpr.size())
            return false;
        return true;
    }
    else
    {
        return errMsg.data();
    }
}

std::variant<bool, std::string> num()
{
    return  match(isdigit(rowExpr[lookhead]), 
            std::format("expected a digit but got {}",
            rowExpr[lookhead]));
}


std::optional<std::string> expr()
{
    decltype(auto) numRet = num();
    if (numRet.index() == 1)
        return std::format("num error: {}", get<1>(numRet));
    if (!get<0>(numRet))    // num情况
        return {};

    decltype(auto) matchRet = match(rowExpr[lookhead] == '+',
            std::format("expected a + but got {}", rowExpr[lookhead]));
    if (matchRet.index() == 1)
        return std::format("match error: {}", get<1>(matchRet));
    if (!get<0>(numRet))
        return std::format("char + at end of expritation");

    decltype(auto) recurRet = expr();
    if (recurRet.has_value())
        return std::format("expr error: {}", recurRet.value());

    return {};
}

int main()
{
    lookhead = 0;
    std::cin >> rowExpr;
    if (decltype(auto) ret = expr(); ret.has_value())
        std::println("expr error: {}", ret.value()); 
    else
        std::println("success");
}

