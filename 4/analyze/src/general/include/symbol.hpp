#pragma once
#include <string>

namespace analyze
{

class Symbol
{
public:
	enum SymbolType
	{
		terminal,
		no_terminal,
		other
	};

	Symbol(SymbolType terminal, std::string_view sv):
		m_type { terminal }, m_symbol { sv.data() }
	{}

	Symbol(bool is_terminal_flag, std::string_view sv):
		Symbol(is_terminal(is_terminal_flag), sv){}

	Symbol(const Symbol&) = default;
	Symbol& operator=(const Symbol& rhs) = default;
	Symbol(Symbol&&) = default;
	Symbol& operator=(Symbol&&) = default;
	virtual ~Symbol() = default;

	[[nodiscard]]
	const std::string& symbol_string() const noexcept
	{ return m_symbol; }

	[[nodiscard]]
	SymbolType type() const noexcept
	{ return m_type; }

	[[nodiscard]]
	bool operator==(const Symbol& rhs) const
	{
		return m_type == rhs.m_type
			&& m_symbol == rhs.m_symbol;
	}

	[[nodiscard]]
	auto operator<=>(const Symbol& rhs) const
	{
		if (auto cmp = m_type <=> rhs.m_type; cmp != 0)
			return cmp;
		return m_symbol <=> rhs.m_symbol;
	}

private:
	[[nodiscard]]
	static SymbolType is_terminal(bool flag)
	{
		if (flag)
			return terminal;
		return no_terminal;
	}

private:
	SymbolType m_type;
	std::string m_symbol;
};

}	//namespace analyze

namespace std
{

template<>
struct hash<analyze::Symbol>
{
	std::size_t operator()(const analyze::Symbol& symbol) const
	{
		return std::hash<std::string>{}(symbol.symbol_string()) 
			 ^ std::hash<int>{}(static_cast<int>(symbol.type()));
	}
};

}	//namespace std

