#pragma once
#include "grammar.hpp"

namespace analyze
{

class GrammarFactory
{
public:
	GrammarFactory(IGrammarParser& parser):
		m_parser { parser }
	{}
	template<bool infer_empty = false>
	tl::expected<Grammar, std::string> create_grammar(std::istream& in)
	{
		auto parse_ret = m_parser.parse(in);
		if (!parse_ret)
			return parse_ret;
		auto& grammar = parse_ret.value();
		if (!grammar.validate_noterminals())
		{
			return tl::make_unexpected("a no terminator is undefined");
		}
		
		//在FIRST实现中，不可避免的重复infer_empty类似的逻辑，
		//所以infer_empty直接被功能更全的First::find_first_recursive取代
		if constexpr (infer_empty)
		{
			grammar.infer_empty_string();
		}

		return grammar;
	}
private:
	const IGrammarParser& m_parser;
};

}	//namespace analyze
