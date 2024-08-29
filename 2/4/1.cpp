#include <cctype>
#include <iostream>
#include <print>
#include <system_error>
#include <tl/expected.hpp>

enum class ExprError
{
    success = 0,  
    match_error,
    expr_unexpected_word,
    expr_end_invalid
};

const auto& expr_error_category()
{
    static class : public std::error_category
    {
        const char* name() const noexcept override
        {
            return "expr_error_category";
        }

        std::string message(int ev) const override
        {
            #define ExprErrorCase(x) \
            case ExprError::x: return #x

            switch(static_cast<ExprError>(ev))
            {
            ExprErrorCase(success);
            ExprErrorCase(expr_unexpected_word);
            ExprErrorCase(expr_end_invalid);
            default:
                return "Unkown error";
            }

            #undef ExprErrorCase
        }
    } instance;

    return instance;
}

std::error_code make_error_code(ExprError ev)
{
    return { static_cast<int>(ev), expr_error_category() };
}

namespace std
{
    template<>
    struct is_error_code_enum<ExprError> : true_type {};
};

std::string rowExpr;
size_t lookhead;

tl::expected<bool, std::error_code> match(bool expr)
{
    [[unlikely]] if (lookhead >= rowExpr.size())
        throw std::logic_error { "未处理lookhead到达尾部" };

    if (expr)
    {
        lookhead++;
        if (lookhead == rowExpr.size())
            return true;
        return false;
    }

    return tl::unexpected { make_error_code(ExprError::match_error) };
}

tl::expected<bool, std::error_code> expr()
{
    //alpha
    if (auto ret = match(isalpha(rowExpr[lookhead])
                || isdigit(rowExpr[lookhead])); ret)
    {
        return ret;
    }

    //+SS -SS
    if (auto ret = match('+' == rowExpr[lookhead]
        || '-' == rowExpr[lookhead]); ret)
    {
        if (*ret)
            return tl::unexpected { make_error_code(ExprError::expr_end_invalid) };

        auto exprRet = expr();
        if (!exprRet)
            return exprRet;
        if (*exprRet)
            return tl::unexpected { make_error_code(ExprError::expr_end_invalid) };
        
        exprRet = expr();
        if (!exprRet)
            return exprRet;
        if (*exprRet)
            return true;

        return ret;
    }

    //excepted
    return tl::unexpected { make_error_code(ExprError::expr_unexpected_word) };
}

int main()
{
    lookhead = 0;
    std::cin >> rowExpr;
    auto ret = expr();
    if (ret)
    {
        if (!*ret)
            std::println("syntax error behind [{}] {}",
                    lookhead, rowExpr[lookhead]);
        else
            std::println("success");
    }
    else
    {
        std::println("syntax error: {}", ret.error().message());
    }
}

