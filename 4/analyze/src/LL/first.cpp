#include "first.hpp"

namespace analyze
{

FirstSet::FirstSet(const Grammar& grammar):
	m_grammar { grammar }
{
	construct();
}

auto FirstSet::get_set(const Symbol& symbol) const
	-> const std::unordered_set<Symbol>
{
	
}

bool FirstSet::has_set(const Symbol& symbol) const
{
	return m_firstSet.contains(symbol);
}

void FirstSet::construct()
{
	auto start_symbol = check_expect(m_grammar.get_start());
	
}

auto FirstSet::find_recursive(Symbol& symbol) -> SymbolsSet
{
	SymbolsSet result;
	if (symbol.type() == Symbol::terminal)
	{
		result.insert(symbol);
		m_firstSet.emplace(symbol, result);
		return result;
	}

	auto set = check_expect(m_grammar.find(symbol)).get();
	
	for(auto rule : set)
	{
		//X可以推出空集
		if (rule.empty())
		{
			result.insert(empty_symbol);	
			continue;
		}
	}

	if (result.empty())
		throw FirstSetException {
			std::format("left symbol {} first set is empty", symbol.symbol_string())
		};

	m_firstSet.emplace(symbol, result);
	return result;
}

}	//namespace analyze
