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
	for (const auto& [ sptr, set ] : m_followpos)
	{
		std::print(os, "{} :\t", sptr->leavePtr->chr);
		for ( const auto& sptr : set)
		{
			std::print(os, "{} ", sptr->leavePtr->chr);
		}
		os << std::endl;
	}
}

auto DFA::cal_nullable(sptr_t sptr) -> bool
{
	bool ret;
	bool leftRet = false, rightRet = false;
	//确保每个分支节点都能递归
	if (sptr->leftChild != nullptr)
		leftRet = cal_nullable(sptr->leftChild); 
	if (sptr->rightChild != nullptr)
		rightRet = cal_nullable(sptr->rightChild);

	switch(sptr->nodeType)
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
			std::format("expected node type {}", static_cast<int>(sptr->nodeType))
		};
	}

	m_nullable[sptr] = ret;
	return ret;
}


//函数没有返回值，直接采用firstpos/lastpos map以节省空间
template<bool isFirst>
auto DFA::cal_flpos(sptr_t sptr) -> void
{

	if (sptr->leftChild != nullptr)
		cal_flpos<isFirst>(sptr->leftChild);
	if (sptr->rightChild != nullptr)
		cal_flpos<isFirst>(sptr->rightChild);

	std::unordered_map<sptr_t, std::unordered_set<sptr_t>>* dealMap;
	if constexpr (isFirst)
		dealMap = &m_firstpos;
	else
		dealMap = &m_lastpos;

	switch(sptr->nodeType)
	{
	case SyntaxTree::Node::CAT: {
		sptr_t c1, c2;
		if constexpr (isFirst)
		{
			c1 = sptr->leftChild;
			c2 = sptr->rightChild;
		}
		else
		{
			c1 = sptr->rightChild;
			c2 = sptr->leftChild;
		}

		auto nullableItr = check_and_get_table_elements(c1, m_nullable);
		// if (nullable(c_1/ c_2)
		if (nullableItr->second)
		{
			auto retItr = check_and_get_table_elements(c1, *dealMap);
			auto retSet = retItr->second;
			const auto& tmpSet = (*dealMap)[c2];
			retSet.insert(tmpSet.cbegin(), tmpSet.cend());
			dealMap->insert({sptr, retSet});
		}
		else
		{
			auto retItr = check_and_get_table_elements(c1, *dealMap);
			auto [_, success] = dealMap->insert({sptr, retItr->second});
			assert(success);
		}

		return;
	}
	case SyntaxTree::Node::OR:{
		assert(sptr->leftChild != nullptr);
		assert(sptr->rightChild != nullptr);
		auto leftItr =
			check_and_get_table_elements(sptr->leftChild, *dealMap);
		auto rightItr =
			check_and_get_table_elements(sptr->rightChild, *dealMap);

		auto dealSet = leftItr->second;
		dealSet.insert(rightItr->second.cbegin(), rightItr->second.cend());
		dealMap->insert({sptr, std::move(dealSet)});
		return;
	}
	case SyntaxTree::Node::STAR: {
		assert(sptr->leftChild != nullptr);
		auto dealItr =
			check_and_get_table_elements(sptr->leftChild, *dealMap);
		dealMap->insert({sptr, dealItr->second});
		return;
	}
	case SyntaxTree::Node::LEAVE: {
		auto [_, success] = dealMap->insert({sptr, std::unordered_set{sptr}});
		assert(success);
		return;
	}
	case SyntaxTree::Node::END: {
		if constexpr (isFirst)
		{
			auto [_, success] = m_firstpos.insert({sptr, std::unordered_set{sptr}});
			assert(success);
		}
		else
		{
			m_lastpos.insert({sptr, {}});
		}
		return;
	}
	default:
		throw std::logic_error {
			std::format("expected node type {}", static_cast<int>(sptr->nodeType))
		};
	}

}

auto DFA::cal_followpos(sptr_t sptr) -> void
{
	auto& left = sptr->leftChild;
	auto& right = sptr->rightChild;
	if (left != nullptr)
		cal_followpos(left);
	if (right != nullptr)
		cal_followpos(right);
	
	if (sptr->nodeType == SyntaxTree::Node::CAT)
	{
		auto lpleftItr = check_and_get_table_elements(left, m_lastpos);
		auto lpRightItr = check_and_get_table_elements(right, m_firstpos);
		const auto& lpRight = lpRightItr->second;
		for (auto puptrLeft : lpleftItr->second)
		{
			auto& fp = m_followpos[puptrLeft];
			fp.insert(lpRight.cbegin(), lpRight.cend());
		}
	}
	else if (sptr->nodeType == SyntaxTree::Node::STAR)
	{
		auto lprtItr = check_and_get_table_elements(sptr, m_lastpos);
		auto fprtItr = check_and_get_table_elements(sptr, m_firstpos);
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

	m_begin = make_shared<vertex>(m_firstpos.at(m_tree.m_root));
	m_vertexTable.insert({m_begin, m_begin->contains(m_tree.m_end)});

	std::queue<std::shared_ptr<vertex>> dataStats;
	dataStats.push(m_begin);

	while(!dataStats.empty())
	{
		auto S = dataStats.front();
		dataStats.pop();
		for (char c : charTable)
		{
			auto U = std::make_shared<vertex>();
			for (sptr_t sptr : *S)
			{
				if (sptr->leavePtr->chr != c)
					continue;
				auto flwItr = m_followpos.find(sptr);
				if (flwItr == m_followpos.end())
					continue;
				const auto& set = flwItr->second;
				U->insert(set.cbegin(), set.cend());
			}
			if (U->empty())
				continue;

			//需要处理相等问题
			auto [ insItr, insSuccess ] = m_vertexTable.insert({U, U->contains(m_tree.m_end)});
			auto& Ulike = insItr->first;
			if (insSuccess && !U->empty())
				dataStats.push(Ulike);
			m_graph.insert( { { S, c }, Ulike });
		}
	}
}

void DFA::minimize()
{
	auto division = this->initial_division();
	auto& charTable = m_tree.m_charTrick;
	for (State& state : division)
	{
		ShdVertex base;
		for (char symbol : charTable)
		{
			for (ShdVertex shdVertex : state)
			{
				auto next = get_next(shdVertex, symbol);
				
				if (next == nullptr)
				{
					
				}
			}
		}
	}
}

auto DFA::initial_division() -> StateDivision
{
	State acceptSet, nonAcceptSet;
	for (auto [shdVertex, accept] : m_vertexTable)
	{
		//可接受态
		if (accept)
			acceptSet.insert(shdVertex);
		else
			nonAcceptSet.insert(shdVertex);
	}
	StateDivision ret;
	ret.push_back(std::move(acceptSet));
	ret.push_back(std::move(nonAcceptSet));
	return ret;
}

auto DFA::get_next(ShdVertex, char) -> ShdVertex
{
	
}

void DFA::add_new_state(ShdVertex shdVertex)
{
	State state { shdVertex };
	
}


}	//namespace simple_regex


