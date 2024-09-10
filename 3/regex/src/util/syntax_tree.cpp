#include "syntax_tree.hpp"
#include <format>
#include <cassert>

namespace simple_regex
{

auto SyntaxTree::prase_regex(std::string_view sv)
	-> tl::expected<void, std::string>
{
	auto ret = parse(sv);
	if (!ret)
		return tl::make_unexpected( ret.error() );
	m_root = std::move(*ret);
	
	return {};
}

auto SyntaxTree::parse(std::string_view sv)
	-> tl::expected<std::unique_ptr<Node>, std::string>
{
	//需要在前添加转译字符的字符集
	static constexpr char charSet[] = {'*', '|', '(', ')', '\\'};

	if (sv.length() == 0)
		return nullptr;

	//记录相同优先级的第一个匹配,
	//注意catIdx指向连接的第二各
	ssize_t starIdx = -1, orIdx = -1, catIdx = -1;
	//注意：转义字符和括号表达式指向其最后一个字符
	size_t preSymbol = std::numeric_limits<ssize_t>::max();
	for (size_t idx = 0; idx < sv.length(); )
	{
		switch(sv[idx])
		{
		case '\\': {
			if (idx == sv.length())
				return tl::make_unexpected(
					"regex expression end with an expected `\\`"
				);

			++idx;
			if (!std::any_of(std::begin(charSet), std::end(charSet), [&]
					(char x) { return sv[idx] == x; }))
			{
				return tl::make_unexpected(
					std::format("\\ with an invalid charater {} in pos {}", sv[idx], idx)
				);
			}
			//判断前一个是否为symbol项
			if (catIdx < 0 && preSymbol == idx-1)
			{
				catIdx = idx;
			}
			preSymbol = idx;
			break;
		}

		case '(':  {
			auto ret = pattern_pth(sv, idx);
			if (!ret)
				return tl::make_unexpected(ret.error());
			//串被一个括号包围
			if (idx == 0 && *ret == sv.length())
			{
				sv = sv.substr(1, sv.length()-2);
				if (sv.length() == 0)
					return nullptr;
				continue;
			}
			if (catIdx < 0 && preSymbol == idx-1)
				catIdx = idx;
			idx = *ret;
			preSymbol = idx-1;
			continue;
		}

		case ')':
			return tl::make_unexpected(
				std::format("right parenthese in pos {} does not match", idx)
			);
			break;

		case '*':
			if (preSymbol != idx-1)
				return tl::make_unexpected(
					std::format("Control character * in pos {} must follow with a normal symbol",
						idx)
				);
			if (starIdx < 0)
				starIdx = idx;
			break;

		case '|':
			if (idx == 0 || idx == sv.length() -1)
				return tl::make_unexpected(
					std::format("Control character | in pos {} with an empty expr", idx)
				);
			if (orIdx < 0)
				orIdx = idx;
			break;
			
		//普通字符
		default:
			if (catIdx < 0 && preSymbol == idx-1)
			{
				catIdx = idx;
			}
			preSymbol = idx;
			break;
		}
		idx++;
	}

	if (orIdx > 0)
	{
		assert(sv.length()-orIdx-1 < sv.length());
		auto ret = parse_cat(sv, 0, orIdx, orIdx+1, sv.length()-orIdx-1, Node::OR);
		if (!ret)
			return tl::make_unexpected(ret.error());
		//为了隐式转换成tl::expected, 这里用std::move转换成右值是必须的
		return std::move(*ret);
	}

	if (catIdx > 0)
	{
		assert(sv.length()-catIdx < sv.length());
		auto ret = parse_cat(sv, 0, catIdx, catIdx, sv.length() - catIdx, Node::CAT);
		if (!ret)
			return tl::make_unexpected(ret.error());
		return std::move(*ret);
	}
	
	if (starIdx > 0)
	{
		assert(sv.length()-starIdx-1 < sv.length());
		auto ret = parse_cat(sv, 0, starIdx, starIdx+1, sv.length()-starIdx-1, Node::STAR);
		if (!ret)
			return tl::make_unexpected(ret.error());
		return std::move(*ret);
	}
	
	if (preSymbol >= sv.length()) [[unlikely]]
	{
		throw std::logic_error { "No symbols were caught" };
	}

	m_symbol2idx[sv[preSymbol]].push_back(m_leavesCounter);
	//只有单个符号时会达到此分支
	auto ret = std::make_unique<Node>(Node::LEAVE, sv[preSymbol], m_leavesCounter);
	m_leavesCounter++;

	return ret;
}

auto SyntaxTree::parse_cat(std::string_view sv, size_t leftBeg, size_t leftLen,
	size_t rightBeg, size_t rightLen, Node::NodeType type)
		-> tl::expected<std::unique_ptr<Node>, std::string>
{
	auto left = parse(sv.substr(leftBeg, leftLen));
	if (!left) return tl::make_unexpected(left.error());
	auto right = parse(sv.substr(rightBeg, rightLen));
	if (!right) return tl::make_unexpected(right.error());

	return std::make_unique<Node>(type, std::move(*left), std::move(*right));
}

auto SyntaxTree::pattern_pth(std::string_view sv, size_t idx) const
		-> tl::expected<size_t, std::string>
{
	ssize_t pthCounter = 1;
	assert(sv[idx] == '(');
	assert(idx < sv.length());

	size_t i = idx + 1;
	for(; pthCounter > 0; i++)
	{
		if (i == sv.length())
			return tl::make_unexpected(
				std::format("left parenthese in pos {} does not match", idx)
			);
		if (sv[i] == '(')
			pthCounter++;

		if (sv[i] == ')')
		{
			pthCounter--;
			if (pthCounter < 0)
				return tl::make_unexpected(
					std::format("right parenthese in pos {} does not match", i)
				);
		}
	}

	return i;
}

} // namespace simple_regex
 
