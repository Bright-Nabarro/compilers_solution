#include "dfa.hpp"

namespace simple_regex 
{

void DFA::create_graph(SyntaxTree&& tree)
{
	m_tree = std::move(tree);
	if (m_tree.m_root == nullptr)
		return;

	cal_nullable(m_tree.m_root);
	//firstpos
	cal_flpos<true>(m_tree.m_root);
	//lastpos
	cal_flpos<false>(m_tree.m_root);
	
	cal_followpos(m_tree.m_root);
}

void DFA::display(std::ostream& os)
{
	(void)os;
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

	std::unordered_map<puptr_t, std::unordered_set<puptr_t>>* dealMap;
	if constexpr (isFirst)
		dealMap = &m_firstpos;
	else
		dealMap = &m_lastpos;

	switch(uptr->nodeType)
	{
	case SyntaxTree::Node::CAT: {
		puptr_t c1, c2;
		if constexpr (isFirst)
		{
			c1 = &uptr->leftChild;
			c2 = &uptr->rightChild;
		}
		else
		{
			c1 = &uptr->rightChild;
			c2 = &uptr->leftChild;
		}

		auto nullableItr = check_and_get_table_elements(c1, m_nullable);
		// if (nullable(c_1/ c_2)
		if (nullableItr->second)
		{
			auto retItr = check_and_get_table_elements(c1, m_firstpos);
			auto retSet = retItr->second;
			const auto& tmpSet = m_firstpos[c2];
			retSet.insert(tmpSet.cbegin(), tmpSet.cend());
			dealMap->insert({&uptr, retSet});
		}
		else
		{
			auto retItr = check_and_get_table_elements(c1, m_firstpos);
			assert(!dealMap->contains(&uptr));
			dealMap->insert({&uptr, retItr->second});
		}

		return;
	}
	case SyntaxTree::Node::OR:{
		assert(uptr->leftChild != nullptr);
		assert(uptr->rightChild != nullptr);
		auto leftItr =
			check_and_get_table_elements(&uptr->leftChild, m_firstpos);
		auto rightItr =
			check_and_get_table_elements(&uptr->rightChild, m_firstpos);
		auto dealSet = leftItr->second;
		dealSet.insert(rightItr->second.cbegin(), rightItr->second.cend());
		dealMap->insert({&uptr, std::move(dealSet)});
		return;
	}
	case SyntaxTree::Node::STAR: {
		assert(uptr->leftChild != nullptr);
		auto dealItr =
			check_and_get_table_elements(&uptr->leftChild, m_firstpos);
		dealMap->insert({&uptr, dealItr->second});
		return;
	}
	case SyntaxTree::Node::LEAVE:
	case SyntaxTree::Node::END: {
		assert(!dealMap->contains(&uptr));
		dealMap->insert({&uptr, std::unordered_set{&uptr}});
		return;
	}
	default:
		throw std::logic_error {
			std::format("expected node type {}", static_cast<int>(uptr->nodeType))
		};
	}

}

auto DFA::cal_followpos(uptr_t& uptr) -> void
{
	auto& left = uptr->leftChild;
	auto& right = uptr->rightChild;
	if (left != nullptr)
		cal_followpos(left);
	if (right != nullptr)
		cal_followpos(right);
	
	if (uptr->nodeType == SyntaxTree::Node::CAT)
	{
		auto lpleftItr = check_and_get_table_elements(&left, m_lastpos);
		auto lpRightItr = check_and_get_table_elements(&right, m_lastpos);
		const auto& lpRight = lpRightItr->second;
		for (auto puptrLeft : lpleftItr->second)
		{
			auto& fp = m_followpos[puptrLeft];
			fp.insert(lpRight.cbegin(), lpRight.cend());
		}
	}
	else if (uptr->nodeType == SyntaxTree::Node::STAR)
	{
		auto lprtItr = check_and_get_table_elements(&uptr, m_lastpos);
		auto fprtItr = check_and_get_table_elements(&uptr, m_firstpos);
		const auto& fprt = fprtItr->second;
		for (auto lpItem : lprtItr->second)
		{
			auto& fpSet = m_followpos[lpItem];
			fpSet.insert(fprt.cbegin(), fprt.cend());
		}
	}
}

}	//namespace simple_regex

