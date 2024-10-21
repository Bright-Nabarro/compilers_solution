#include "follow.hpp"

namespace analyze
{

auto FollowSet::find_follow(const Symbol& symbol) const ->std::optional<SymbolsSet>
{
	auto itr = m_follow_map.find(symbol);
	if (itr == m_follow_map.cend())
		return std::nullopt;
	return itr->second;
}

bool FollowSet::in_follow_set(const Symbol& find, const Symbol& valid) const
{
	auto itr = m_follow_map.find(find);
	if (itr == m_follow_map.cend())
		return false;
	return itr->second.contains(valid);
}

void FollowSet::constructor()
{
	auto no_terminals = m_grammar.no_terminals();
	for (const auto& symbol : no_terminals)
	{
		m_follow_map.emplace(symbol, std::unordered_set<Symbol>{});
	}

	auto start_expeced = m_grammar.get_start();
	if (!start_expeced)
		throw std::logic_error { "grammar start symbol unset" };
	const auto& start_symbol = start_expeced.value();
	// 1)
	m_follow_map.at(start_symbol).insert(end_symbol);
	fill_follow();
}

void FollowSet::fill_follow()
{
	bool next_loop = true;
	while (next_loop)
	{
		next_loop = false;
		for (auto& [follow_symbol, follow_set] : m_follow_map)
		{
				
		}
	}
}

bool FollowSet::traverse_no_terminal_rules(const Symbol& no_terminal)
{
	bool result;
	const auto& rules = check_expect(m_grammar.find(no_terminal)).get();
	for (const auto& rule : rules)
	{
		auto empty_itr = std::prev(rule.cend());
		bool empty_ctn = true;
		for (auto symbol_itr = std::next(rule.crbegin());
			 symbol_itr != rule.crend();
			 ++symbol_itr)
		{
			if (symbol_itr->type() != Symbol::no_terminal)
				continue;
			// 2)
			auto first_ret = m_first.find_first(*std::next(symbol_itr));
			if (!first_ret.has_value())
				throw std::logic_error { "unkown unterminal in first set" };
			auto first_set = std::move(first_ret.value());
			auto erase_count = first_set.erase(Grammar::empty_symbol);
			if (erase_count && empty_ctn)
				--empty_itr;
			else
				empty_ctn = false;

			result = set_insert_to_follow_set(*symbol_itr, first_set);
		}
		// 3) && 4)
		for (auto symbol_itr = empty_itr;
			 symbol_itr != rule.cend();
			 ++symbol_itr)
		{
		}
	}

	return result;
}

bool FollowSet::set_insert_to_follow_set(const Symbol& symbol, std::unordered_set<Symbol>& set)
{
	auto& follow_set = m_follow_map.at(symbol);
	bool ret = false;
	for (auto& first_symbol : set)
	{
		auto [itr, success] = follow_set.insert(std::move(first_symbol));
		ret = success;
	}
	return ret;
}

}	//namespace analyze
