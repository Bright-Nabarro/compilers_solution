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
    Int,
    Float,
    Id,
    KeyWord,
    Operator,
};

std::string token_type_to_str(TokenType token)
{
    switch(token)
    {
    case TokenType::Int:
        return "integer";
    case TokenType::Float:
        return "float";
    case TokenType::Id:
        return "id";
    case TokenType::KeyWord:
        return "keyword";
    case TokenType::Operator:
        return "operator";
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
};

class Lexer
{
public:
    Lexer(std::string_view sv) :
        m_pos {0}, m_input { sv }
    {
        initial_keyword();
    }

    [[nodiscard]]
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

            if (opr(result)) continue;
			
			if (skip_other()) continue;
            
            return tl::unexpected { std::format("unmatched word pos:[{}] char:`{}`", m_pos, m_input[m_pos]) };
        }
        
        return result;
    }

private:
    [[nodiscard]]
    bool space()
    {
        if (isspace(m_input[m_pos]))    //space
        {
            m_pos++;
            return true;
        }
        return false;
    }

    [[nodiscard]]
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

    [[nodiscard]]
    bool num(std::vector<Token>& result)
    {
        bool isFloat = false;
        
        //允许单独.开头的浮点数
        if (m_input[m_pos] == '.')
        {
            if (++m_pos == m_input.size())
                return false;
            isFloat = true;
        }

        if (isdigit(m_input[m_pos]))   //num 
        {
            size_t beg = m_pos;
            while(true)
            {
                if (++m_pos == m_input.size())
                    break;

                if (m_input[m_pos] == '.')
                    isFloat = true;
                else if (!isdigit(m_input[m_pos]))
                    break;
            }
            assert(m_pos >= beg);

            if (isFloat)
            {
                result.emplace_back(TokenType::Float,
                    std::string{m_input.cbegin() + beg, m_input.cbegin() + m_pos});
            }
            else
            {
                result.emplace_back(TokenType::Int,
                    std::string{m_input.cbegin() + beg, m_input.cbegin() + m_pos});
            }
            return true;
        }
        return false;
    }

    [[nodiscard]]
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
                result.push_back(std::move(token));
            }
            else
            {
                result.push_back(itr->second);
            }
            return true;
        }
        return false;
    }
    
    //不涉及语法处理, 其中!=匹配失败直接返回false
    [[nodiscard]]
    bool opr(std::vector<Token>& result)
    {
        //如果后续匹配到'='返回true, 其他情况返回false
        auto check_post = [&, this](){
            if (++m_pos == m_input.size())
                return false;
            if (m_input[m_pos] == '=')
            {
                ++m_pos;
                return true;
            }
            return false;
        };

        const size_t beg = m_pos;
        switch (m_input[beg])
        {
        case '<':
            check_post();
            result.emplace_back(TokenType::Operator,
                std::string { m_input.cbegin() + beg, m_input.cbegin() + m_pos });
            return true;
        case '>':
            check_post();
            result.emplace_back(TokenType::Operator,
                std::string { m_input.cbegin() + beg, m_input.cbegin() + m_pos });
            return true;
        case '=':
            if (check_post())
            {
                result.emplace_back(TokenType::Operator,
                    std::string { m_input.cbegin() + beg, m_input.cbegin() + m_pos });
                return true;
            }
            else 
                return false;
        case '!':
            if (check_post())
            {
                result.emplace_back(TokenType::Operator,
                    std::string { m_input.cbegin() + beg, m_input.cbegin() + m_pos });
                return true;
            }
            else 
                return false;
        default:
            return false;
        }
    }
    
    void initial_keyword()
    {
       // m_wordsTable["true"] = Words{ TokenType::True, "true" };
        static const std::array wordsList {
            Token { TokenType::KeyWord, "true" },
            Token { TokenType::KeyWord, "false" },
			Token { TokenType::KeyWord, "if" },
			Token { TokenType::KeyWord, "else" },
        };

        for (const auto& words: wordsList)
            m_wordsTable.insert({ words.value, words });
    }

	bool skip_other()
	{
		switch (m_input[m_pos])
		{
		case '(':
		case ')':
		case '{':
		case '}':
		case ';':
			++m_pos;
			return true;
		default:
			return false;
		}
	}

private:
    size_t m_pos;
    std::string_view m_input;
    std::unordered_map<std::string, Token> m_wordsTable;
};

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		std::println(stderr, "need a file");
		return 1;
	}
    std::ifstream inFile { argv[1] };
	if (!inFile)
	{
		std::println(stderr, "expected inFile");
		return 1;
	}

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
        std::println("< {}, `{}` >", token_type_to_str(x.type), x.value);
    }
	
	return 0;
}

