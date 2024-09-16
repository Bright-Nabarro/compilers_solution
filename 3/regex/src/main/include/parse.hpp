#pragma once
#include "dfa.hpp"

namespace simple_regex
{

class Parse
{
public:
	struct ParseString
	{
		std::string context;
		size_t begPos;
		size_t endPos;
	};
	Parse(DFA&& dfa) : m_dfa { std::move(dfa) }{}
	[[nodiscard]]
	auto parse_stream(std::istream& is)
		-> tl::expected<void, std::string>;
	auto get_parse_string_list() const
		-> const std::vector<ParseString>&;
private:
	void construct_parse_string(
		std::string::iterator contentBegin,
		std::string::iterator beg,
		std::string::iterator end);
private:
	DFA m_dfa;
	std::vector<ParseString> m_praseContext;
};

}	//namespace simple_regex


