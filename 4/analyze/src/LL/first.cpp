#include "first.hpp"

namespace analyze
{

FirstSet::FirstSet(const Grammar& grammar):
	m_grammar { grammar }
{
	construct();
}

void FirstSet::construct()
{
	auto no_terminals = m_grammar.no_terminals();
	for (const auto& symbol: no_terminals)
		find_first_recursive(symbol);
}


auto FirstSet::find_first(const Symbol& symbol) const -> std::optional<SymbolsSet>
{
	std::optional<SymbolsSet> ret;
	auto itr = m_first_set.find(symbol);
	if (m_first_set.cend() == itr)
		return std::nullopt;
	return itr->second;
}

bool FirstSet::in_firstset(const Symbol& find, const Symbol& check) const
{
	auto itr = m_first_set.find(find);
	if (m_first_set.cend() == itr)
		return false;
	return itr->second.contains(check);
}

auto FirstSet::find_first_recursive(const Symbol& symbol) -> SymbolsSet
{
	//函数执行逻辑类似于`Grammar::infer_empty_string`, 
	//不同之处在于`terminal symbol`和`empty symbol`也会插入`this->m_first_set`
	SymbolsSet result;
	
	// 1) && 3)
	if (symbol.type() == Symbol::terminal || symbol == Grammar::empty_symbol)
	{
		result.insert(symbol);
		m_first_set.emplace(symbol, result);
		return result;
	}

	auto rules_set = check_expect(m_grammar.find(symbol)).get();

	//先行插入，作为已经遍历的标记
	auto [m_set_itr, success] = m_first_set.emplace(symbol, SymbolsSet{});
	if (!success)
	{
		return m_set_itr->second;
	}

	// 2)
	for (const auto& rules : rules_set)
	{
		for (auto symbol : rules)
		{
			//这里不进行terminal判断，交给递归深处, 也就是上方的 1) && 3)
			auto ret_set = find_first_recursive(symbol);
			bool infer_empty = ret_set.contains(Grammar::empty_symbol);
			result.merge(ret_set);
			if (!infer_empty)
				break;
		}
	}

	if (m_set_itr == m_first_set.end())
		throw FirstSetException { "invalid m_set_itr" };
	m_set_itr->second.insert(result.cbegin(), result.cend());
	return result;
}

}	//namespace analyze

