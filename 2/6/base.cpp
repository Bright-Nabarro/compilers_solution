#include <array>
#include <string>
#include <string_view>
#include <unordered_map>

enum class TokenType {
    Num,
    Id,
    True,
    False,
};

struct Token
{
    const TokenType type;
    explicit Token(TokenType t): type { t } {}
};

struct Num: public Token
{
    const int64_t value;
    explicit Num(int64_t v):
        Token { TokenType::Num }, value { v }{}
};

struct Words: public Token
{
    const std::string lexeme;
    explicit Words(TokenType t, std::string_view sv):
        Token { t }, lexeme { sv }{}
};

class Lexer
{
public:
    Lexer(std::string_view filename)
    {
        
    }
private:
    void initial_keyword()
    {
       // m_wordsTable["true"] = Words{ TokenType::True, "true" };
        std::array wordsList {
            Words { TokenType::True,    "true" },
            Words { TokenType::False,   "false" },
        };

        for (const auto& words: wordsList)
            m_wordsTable.insert({ words.lexeme, words });
    }

private:
    size_t m_pos;
    std::string m_input;
    std::unordered_map<std::string, Words> m_wordsTable;
};


