#include "dfa.hpp"
#include <queue>

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

	construct_graph();
}

void DFA::display_graph(std::ostream& os) const
{
	using namespace std::string_literals;
	//将顶点映射成字符表示
	auto vtx2str = std::unordered_map<std::shared_ptr<vertex>, std::string>{};
	size_t vtx2strCounter = 1;
	for (const auto& [ vtx, end ] : m_vertexTable)
	{
		if (vtx->empty())
			vtx2str.insert(make_pair(vtx, "empty"s ));
		else
			vtx2str.insert(make_pair(vtx, std::to_string(vtx2strCounter++)));
	}
	
	std::println(os, "``` graphviz");
	std::println(os, "digraph DFA{} {{", reinterpret_cast<const void*>(this));
	//开始标记
	std::println(os, "BEGIN_MARK -> {} [label = \"first\"]", vtx2str.at(m_begin));
	std::println(os, "BEGIN_MARK [shape = none, label = \"\"]");
 	//节点
	for (const auto& [ vtx, isEnd ] : m_vertexTable)
	{
		if (isEnd)
			std::println(os, "{} [shape = doublecircle]", vtx2str.at(vtx));
		else
			std::println(os, "{} [shape = circle]", vtx2str.at(vtx));
	}
	
	//图
	for (const auto& [ pair, vtxTo ] : m_graph)
	{
		const auto& [ vtxFrom, edge ] { pair };
		std::println(os, "{0} -> {2} [ label = {1} ]",
				vtx2str.at(vtxFrom), edge, vtx2str.at(vtxTo));
	}

	std::println(os, "}}");
	std::println(os, "```");
}

void DFA::display_followpos(std::ostream& os) const
{
	for (const auto& [ puptr, set ] : m_followpos)
	{
		std::print(os, "{} :\t", (*puptr)->leavePtr->chr);
		for ( const auto& puptr : set)
		{
			std::print(os, "{} ", (*puptr)->leavePtr->chr);
		}
		os << std::endl;
	}
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
			auto retItr = check_and_get_table_elements(c1, *dealMap);
			auto retSet = retItr->second;
			const auto& tmpSet = (*dealMap)[c2];
			retSet.insert(tmpSet.cbegin(), tmpSet.cend());
			dealMap->insert({&uptr, retSet});
		}
		else
		{
			auto retItr = check_and_get_table_elements(c1, *dealMap);
			auto [_, success] = dealMap->insert({&uptr, retItr->second});
			assert(success);
		}

		return;
	}
	case SyntaxTree::Node::OR:{
		assert(uptr->leftChild != nullptr);
		assert(uptr->rightChild != nullptr);
		auto leftItr =
			check_and_get_table_elements(&uptr->leftChild, *dealMap);
		auto rightItr =
			check_and_get_table_elements(&uptr->rightChild, *dealMap);
		auto dealSet = leftItr->second;
		dealSet.insert(rightItr->second.cbegin(), rightItr->second.cend());
		dealMap->insert({&uptr, std::move(dealSet)});
		return;
	}
	case SyntaxTree::Node::STAR: {
		assert(uptr->leftChild != nullptr);
		auto dealItr =
			check_and_get_table_elements(&uptr->leftChild, *dealMap);
		dealMap->insert({&uptr, dealItr->second});
		return;
	}
	case SyntaxTree::Node::LEAVE: {
		auto [_, success] = dealMap->insert({&uptr, std::unordered_set{&uptr}});
		assert(success);
		return;
	}
	case SyntaxTree::Node::END: {
		if constexpr (isFirst)
		{
			auto [_, success] = m_firstpos.insert({&uptr, std::unordered_set{&uptr}});
			assert(success);
		}
		else
		{
			m_lastpos.insert({&uptr, {}});
		}
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
		auto lpRightItr = check_and_get_table_elements(&right, m_firstpos);
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

auto DFA::construct_graph() -> void
{
	auto& charTable = m_tree.m_charTrick;

	auto& pEnd = m_tree.mp_end;
	m_begin = make_shared<vertex>(m_firstpos.at(&m_tree.m_root));
	m_vertexTable.insert({m_begin, m_begin->contains(pEnd)});

	std::queue<std::shared_ptr<vertex>> dataStats;
	dataStats.push(m_begin);

	while(!dataStats.empty())
	{
		auto S = dataStats.front();
		dataStats.pop();
		for (char c : charTable)
		{
			auto U = std::make_shared<vertex>();
			for (puptr_t puptr : *S)
			{
				if ((*puptr)->leavePtr->chr != c)
					continue;
				auto flwItr = m_followpos.find(puptr);
				if (flwItr == m_followpos.end())
					continue;
				const auto& set = flwItr->second;
				U->insert(set.cbegin(), set.cend());
			}
			if (U->empty())
				continue;

			//需要处理相等问题
			auto [ insItr, insSuccess ] = m_vertexTable.insert({U, U->contains(pEnd)});
			auto& Ulike = insItr->first;
			if (insSuccess && !U->empty())
				dataStats.push(Ulike);
			m_graph.insert( { { S, c }, Ulike });
		}
	}
}

}	//namespace simple_regex


