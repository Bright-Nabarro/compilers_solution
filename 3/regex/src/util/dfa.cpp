#include "dfa.hpp"

namespace simple_regex 
{

void DFA::create_graph(SyntaxTree&& tree)
{
	cal_nullable(m_tree.m_root);
}

void DFA::display(std::ostream& os)
{
}

auto DFA::cal_nullable(uptr_t& uptr) -> bool
{
	bool ret;
	bool leftRet = false, rightRet = false;
	//确保每个分支节点都能递归
	if (uptr->leftChild != nullptr)
		leftRet = cal_nullable(uptr->leftChild); 
	if (uptr->rightChild != nullptr)
		rightRet = cal_nullable(uptr->rightChild);

	switch(uptr->nodeType)
	{
	//OR和CAT必须拥有两个子节点
	case SyntaxTree::Node::CAT:
		// 可以从map中直接提取，返回值仅仅为优化手段
		ret = leftRet && rightRet;
		break;
	case SyntaxTree::Node::OR:
		ret = leftRet || rightRet;
		break;
	case SyntaxTree::Node::STAR:
		ret = true;
		break;
	case SyntaxTree::Node::LEAVE:
	case SyntaxTree::Node::END:
		ret = false;
		break;
	default:
		throw std::logic_error {
			std::format("expected node type {}", static_cast<int>(uptr->nodeType))
		};
	}

	m_nullable[&uptr] = ret;
	return ret;
}


//函数没有返回值，直接采用firstpos/lastpos map以节省空间
template<bool isFirst>
auto DFA::cal_flpos(uptr_t& uptr) -> void
{

	if (uptr->leftChild != nullptr)
		cal_flpos<isFirst>(uptr->leftChild);
	if (uptr->rightChild != nullptr)
		cal_flpos<isFirst>(uptr->rightChild);

	switch(uptr->nodeType)
	{
	case SyntaxTree::Node::CAT: {
		puptr_t c1, c2;
		std::unordered_map<puptr_t, std::unordered_set<puptr_t>>* dealMap;
		if constexpr (isFirst)
		{
			c1 = &uptr->leftChild;
			c2 = &uptr->rightChild;
			dealMap = &m_firstpos;
		}
		else
		{
			c1 = &uptr->rightChild;
			c2 = &uptr->leftChild;
			dealMap = &m_lastpos;
		}

		auto nullablePtr = m_nullable.find(c1);
		if (nullablePtr == m_nullable.end())	[[unlikely]]
			throw std::logic_error {
				"c1 cannot find in nullable table"
			};

		if (nullablePtr->second)
		{
			auto retSet = m_firstpos[c1];
			const auto& tmpSet = m_firstpos[c2];
			retSet.insert(tmpSet.cbegin(), tmpSet.cend());
			dealMap->insert({&uptr, retSet});
		}

		return;
	}
	case SyntaxTree::Node::OR:
		return;
	case SyntaxTree::Node::STAR:
		return;
	case SyntaxTree::Node::LEAVE:
	case SyntaxTree::Node::END:
		return;
	default:
		throw std::logic_error {
			std::format("expected node type {}", static_cast<int>(uptr->nodeType))
		};
	}

}

auto DFA::cal_followpos() -> std::unordered_set<puptr_t>
{
}

}	//namespace simple_regex

