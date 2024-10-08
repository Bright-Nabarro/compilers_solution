#include "syntax_tree.hpp"
#include <format>
#include <cassert>
#include <print>
#include <algorithm>

namespace simple_regex
{

auto SyntaxTree::parse_regex(std::string_view sv)
	-> tl::expected<void, std::string>
{
	reset();
	auto ret = parse(sv);
	if (!ret)
		return tl::make_unexpected( ret.error() );
	if (*ret == nullptr)
		return {};
	//在根的左子树添加#
	m_root = std::make_shared<Node>(SyntaxTree::Node::CAT, *ret,
		std::make_shared<Node>(SyntaxTree::Node::END, '#'));
	m_end = m_root->rightChild;
	return {};
}

void SyntaxTree::display(std::ostream& os) const
{
	std::println(os, "``` graphviz");
	std::println(os, "digraph SyntaxTree{} {{", reinterpret_cast<const void*>(this));
	std::println(os, "node [shape=plaintext]");

	if (m_root != nullptr)
	{
		auto rootStr = nodeType2string(m_root);
		std::println(os, R"("{}" [label = "{}"])", rootStr.value().first, rootStr.value().second);
		display(os, m_root, rootStr.value().first);
	}
	std::println(os, "}}");
	std::println(os, "```\n");
}

void SyntaxTree::display(std::ostream& os, const std::shared_ptr<Node> ptr,
	const std::string& ptrStr) const
{
	auto left = nodeType2string(ptr->leftChild);
	auto right = nodeType2string(ptr->rightChild);

	if (left)
	{
		std::println(os, R"("{}" -> "{}")", ptrStr, left.value().first);
		std::println(os, R"("{}" [label = "{}"])", left.value().first, left.value().second);
	}
	if (right)
	{
		std::println(os, R"("{}" -> "{}")", ptrStr, right.value().first);
		std::println(os, R"("{}" [label = "{}"])", right.value().first, right.value().second);
	}

	if (left)
	{
		display(os, ptr->leftChild, left.value().first);
	}
	if (right)
	{
		display(os, ptr->rightChild, right.value().first);
	}
}

auto SyntaxTree::nodeType2string(const std::shared_ptr<Node> ptr) const ->
	std::optional<std::pair<std::string, std::string>>
{
	if (ptr == nullptr)
		return std::nullopt;

	switch(ptr->nodeType)
	{
	case Node::CAT:
		return std::pair{std::format("cat{}", displayContext.catCounter++), "•"};
	case Node::OR:
		return std::pair{std::format("or{}", displayContext.orCounter++), "|"};
	case Node::STAR:
		return std::pair{std::format("star{}", displayContext.starCounter++), "*"};
	case Node::END:
	case Node::LEAVE: {
		std::string leaveStr = std::format("{} {}", ptr->leavePtr->chr, displayContext.leavesCounter++);
		return std::pair{leaveStr, leaveStr};
	}
	default:
		throw std::invalid_argument(std::format("unkown node type {}",
				static_cast<int>(ptr->nodeType)));
	}
}


auto SyntaxTree::parse(std::string_view sv)
	-> tl::expected<std::shared_ptr<Node>, std::string>
{
	//需要在前添加转译字符的字符集
	static constexpr char charSet[] = {'*', '|', '(', ')', '\\'};

	if (sv.length() == 0)
		return nullptr;

	//or, cat相同优先级的最后一个匹配，保证为左结合,
	//catIdx指向连接的第二个字符
	ssize_t starIdx = -1, orIdx = -1, catIdx = -1;
	//转译字符和括号表达式指向其最后一个字符
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
			if (preSymbol == idx-1)
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
			if (preSymbol == idx-1)
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
			preSymbol = idx;
			break;

		case '|':
			if (idx == 0 || idx == sv.length() -1)
				return tl::make_unexpected(
					std::format("Control character | in pos {} with an empty expr", idx)
				);
			orIdx = idx;
			break;
			
		//普通字符
		default:
			if (preSymbol == idx-1)
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
		return *ret;
	}

	if (catIdx > 0)
	{
		assert(sv.length()-catIdx < sv.length());
		auto ret = parse_cat(sv, 0, catIdx, catIdx, sv.length() - catIdx, Node::CAT);
		if (!ret)
			return tl::make_unexpected(ret.error());
		return *ret;
	}
	
	if (starIdx > 0)
	{
		assert(sv.length()-starIdx-1 < sv.length());
		auto ret = parse_cat(sv, 0, starIdx, starIdx+1, sv.length()-starIdx-1, Node::STAR);
		if (!ret)
			return tl::make_unexpected(ret.error());
		return *ret;
	}
	
	if (preSymbol >= sv.length()) [[unlikely]]
	{
		throw std::logic_error { "No symbols were caught" };
	}

	//只有单个符号时会达到此分支
	auto ret = std::make_shared<Node>(Node::LEAVE, sv[preSymbol]);
	//m_leavesTable.insert(&ret);
	//m_chrTable[sv[preSymbol]].insert(&ret);
	m_charTrick.insert(sv[preSymbol]);

	return ret;
}

auto SyntaxTree::parse_cat(std::string_view sv, size_t leftBeg, size_t leftLen,
	size_t rightBeg, size_t rightLen, Node::NodeType type)
		-> tl::expected<std::shared_ptr<Node>, std::string>
{
	auto left = parse(sv.substr(leftBeg, leftLen));
	if (!left) return tl::make_unexpected(left.error());
	auto right = parse(sv.substr(rightBeg, rightLen));
	if (!right) return tl::make_unexpected(right.error());

	return std::make_shared<Node>(type, *left, *right);
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

void SyntaxTree::reset()
{
	m_root = nullptr;
	//m_leavesTable.clear();
	//m_chrTable.clear();
	m_charTrick.clear();
}

} // namespace simple_regex
 
