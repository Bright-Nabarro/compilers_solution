//#include <array>
#include "parse.hpp"
#include "stream_error.hpp"

namespace simple_regex
{

//istream的初始状态依赖于上层检查
auto Parse::parse_stream(std::istream& is)
		-> tl::expected<void, std::string>
{
	while(is)
	{
		//static constexpr size_t bufferSize = 8192;
		//std::array<char, bufferSize> buffer;
		//is.read(buffer.data(), buffer.size());
		//
		//if (is.gcount() == 0)
		//	return {};
		std::string content;
		std::string line;
		while (std::getline(is, line))
			content += line;

		const auto& graph = m_dfa.get_graph();
		auto curPtr = m_dfa.get_begin();
		auto bufferItr = content.begin();
		auto parseSuccessBeg = bufferItr;
		//此迭代器指向的是有效字符，并非超尾位置，如果无效则指向content.end()
		auto parseSuccessEnd = content.end();		

		//采用最长匹配
		while (bufferItr != content.end())
		{
			auto graphKey = make_pair(curPtr, *bufferItr);
			auto graphItr = graph.find(graphKey);
			if (graphItr != graph.end())
			{
				curPtr = graphItr->second;
				//判断当前节点是否为终态
			 	if (m_dfa.get_vtxTable().at(curPtr))
					parseSuccessEnd = bufferItr;
				++bufferItr;
			}
			//当前字符在图中进入无效状态
			else
			{
				if (parseSuccessEnd != content.end())
				{
					construct_parse_string(content.begin(), parseSuccessBeg, parseSuccessEnd+1);
				}
				else if (parseSuccessBeg == bufferItr)
				{
					++bufferItr;
				}
				curPtr = m_dfa.get_begin();
				parseSuccessBeg = bufferItr;
				parseSuccessEnd = content.end();
			}
		}
		
		if (parseSuccessEnd != content.end())
		{
			construct_parse_string(content.begin(), parseSuccessBeg, parseSuccessEnd+1);
		}
	}
	auto isErrorCode = yq::get_stream_error(is);
	if (isErrorCode != yq::stream_errc::success
	 && isErrorCode != yq::stream_errc::eof)
			return tl::make_unexpected(isErrorCode.message());

	return {};
}

auto Parse::get_parse_string_list() const
		-> const std::vector<ParseString>&
{
	return m_praseContext;
}

void Parse::construct_parse_string(
		std::string::iterator contentBegin,
		std::string::iterator beg,
		std::string::iterator end)
{
	m_praseContext.emplace_back(
		std::string { beg, end },
		std::distance(contentBegin, beg)
	);
}

void Parse::clear_result()
{
	m_praseContext.clear();
}


}	//namespace simple_regex

