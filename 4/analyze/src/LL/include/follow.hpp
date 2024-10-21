#pragma once
#include "grammar.hpp"
#include "first.hpp"

namespace analyze
{

class FollowSet
{
public:
	using SymbolsSet = std::unordered_set<Symbol>;
	inline static const Symbol end_symbol {Symbol::other, "$"};
	FollowSet(const Grammar& grammar, const FirstSet& first):
		m_grammar { grammar }, m_first { first }
	{
		constructor();
	}

	virtual ~FollowSet() = default;
	[[nodiscard]]
	std::optional<SymbolsSet> find_follow(const Symbol& symbol) const;
	[[nodiscard]]
	bool in_follow_set(const Symbol& find, const Symbol& valid) const;
private:
	void constructor();
	/*
	 * 1) FOLLOW(get_start()) += '$'
	 * 2) A -> BC: FOLLOW(B) += (FIRST(C) - empty)
	 * 3) A -> BC: FOLLOW(C) += FOLLOW(C)
	 * 4) A -> BC: if FIRST(A).contains(empty) then FOLLOW(B) += FOLLOW(A)
	 */
	void fill_follow();
	[[nodiscard]]
	bool traverse_no_terminal_rules(const Symbol& no_terminal, SymbolsSet& follow_set);
	[[nodiscard]]
	bool set_insert_to_follow_set(const Symbol& follow_key, std::unordered_set<Symbol>& set);
	
	const Grammar& m_grammar;
	const FirstSet& m_first;
	std::unordered_map<Symbol, SymbolsSet> m_follow_map;
};

}	//namespace analyze
