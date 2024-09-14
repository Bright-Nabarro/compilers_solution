#pragma once
#include "dfa.hpp"

namespace simple_regex
{

class Prase
{
public:
	Prase(DFA&& dfa);
	auto prase_stream(std::istream is) -> tl::expected<void, std::string>;
private:
	DFA m_dfa;

};

}	//namespace simple_regex
