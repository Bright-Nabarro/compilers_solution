#include <array>
#include <string>
#include <string_view>
#include <unordered_map>
#include <tl/expected.hpp>
#include <cctype>
#include <fstream>
#include <print>

using namespace std::string_literals;

enum class TokenType {
    Num,
    Id,
    KeyWord,
};

std::string token_type_to_str(TokenType token)
{
    switch(token)
    {
    case TokenType::Num:
        return "num";
    case TokenType::Id:
        return "id";
    case TokenType::KeyWord:
        return "keyword";
    default:
        return "unkown";
    }
}

struct Token
{
    const TokenType type;
    std::string value;
    Token(TokenType t, std::string_view sv):
        type { t }, value { sv } {}
    Token(TokenType t, std::string&& s):
        type { t }, value { std::move(s) } {}

};

class Lexer
{
public:
    Lexer(std::string_view sv) :
        m_pos {0}, m_input { sv }
    {
        initial_keyword();
    }

    tl::expected<std::vector<Token>, std::string> tokenize()
    {
        std::vector<Token> result;

        while(m_pos < m_input.size())
        {
            if (space()) continue;

            if (num(result)) continue;

            if (auto ret = comments(); ret)
            {
                if (*ret)
                    continue;
            }
            else 
                return tl::unexpected { ret.error() };

            if (words(result)) continue;
            
            return tl::unexpected { std::format("unmatched word pos:[{}] char:{}", m_pos, m_input[m_pos]) };
        }
        
        return result;
    }

private:
    bool space()
    {
        if (isspace(m_input[m_pos]))    //space
        {
            m_pos++;
            return true;
        }
        return false;
    }

    tl::expected<bool, std::string> comments()
    {
        if (m_input[m_pos] != '/')
        {
            return false;
        }

        if (m_input[m_pos + 1] == '/')  //单行注释
        {
            if (++m_pos == m_input.size())
                return tl::unexpected {"text end with a expected /"};
            do {
                if (++m_pos == m_input.size())
                    return true;
            } while(m_input[m_pos] != '\n');
            m_pos++;        //跳过\n
            return true;
        }

        if (m_input[m_pos + 1] == '*') // 多行注释
        {
            if (++m_pos == m_input.size() - 2)
                return tl::unexpected {"text end with a expected /"};

            do {
                if (++m_pos == m_input.size() - 2)
                {
                    return tl::unexpected { "text end with a single left comment pair `/*`" };
                }
            } while(m_input[m_pos] != '*' && m_input[m_pos + 1] != '/');
            m_pos += 2;     //跳过*/
            return true;
        }

        //单纯的`/`符号
        return false;
    }


    bool num(std::vector<Token>& result)
    {
        if (isdigit(m_input[m_pos]))   //num 
        {
            size_t beg = m_pos;
            do {
                m_pos++;
            } while (m_pos != m_input.size() && isdigit(m_input[m_pos]));

            assert(m_pos >= beg);
            std::string value { m_input.cbegin() + beg, m_input.cbegin() + m_pos };
            result.push_back(Token{TokenType::Num, std::move(value)});
            return true;
        }
        return false;
    }

    bool words(std::vector<Token>& result)
    {
        if (isalpha(m_input[m_pos])
            || m_input[m_pos] == '_') //word no digit begin
        {
            size_t beg = m_pos;
            do {
               m_pos++; 
            } while (m_pos != m_input.size() && isalpha(m_input[m_pos]));

            std::string value { m_input.cbegin() + beg, m_input.cbegin() + m_pos};
            auto itr = m_wordsTable.find(value);
            if (itr == m_wordsTable.end())
            {
                Token token { TokenType::Id, std::move(value) };
                m_wordsTable.insert({token.value, token});
                result.push_back(token);
            }
            else
            {
                result.push_back(itr->second);
            }
            return true;
        }
        return false;
    }
    
    void initial_keyword()
    {
       // m_wordsTable["true"] = Words{ TokenType::True, "true" };
        std::array wordsList {
            Token { TokenType::KeyWord, "true"s },
            Token { TokenType::KeyWord, "false"s },
        };

        for (const auto& words: wordsList)
            m_wordsTable.insert({ words.value, words });
    }

private:
    size_t m_pos;
    std::string_view m_input;
    std::unordered_map<std::string, Token> m_wordsTable;
};

int main()
{
    std::ifstream inFile { "input.c" };
    std::string fileContext { std::istreambuf_iterator<char>(inFile),
        std::istreambuf_iterator<char>() };

    Lexer lexer {fileContext};
    auto ret = lexer.tokenize();
    if (!ret)
    {
        std::println("{}", ret.error());
        return 1;
    }

    for (const auto& x : *ret)
    {
        std::println("< {}, {} >", token_type_to_str(x.type), x.value);
    }
}

