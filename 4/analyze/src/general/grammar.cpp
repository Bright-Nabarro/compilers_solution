#include "grammar.hpp"

#include <format>
#include <unordered_set>
#include <print>

namespace analyze
{

tl::expected<Symbol, std::string> Grammar::get_start() const
{
	if (!m_start_symbol.has_value())	[[unlikely]]
		return tl::make_unexpected("start symbol does not spectialized");
	return *m_start_symbol;
}

Symbol Grammar::get_start_unchecked() const
{
	return m_start_symbol.value();
}

auto Grammar::find(const Symbol& symbol) const
		-> tl::expected<std::reference_wrapper<const SymbolsSet>, std::string>
{
	auto itr = m_rules.find(symbol);
	if (itr == m_rules.end())	[[unlikely]]
	{
		return tl::make_unexpected(
			std::format("right {} doesn't exist in rules", symbol.symbol_string())
		);
	}

	return std::cref(itr->second);

}

bool Grammar::add_rule(Symbol left, std::vector<Symbol> right)
{
	auto map_itr = m_rules.find(left);
	if (map_itr == m_rules.end())
	{
		m_rules.emplace(std::move(left), std::unordered_set<std::vector<Symbol>, SymbolListHash>{ std::move(right) });
		return true;
	}
	
	auto [itr, success] = map_itr->second.insert(std::move(right));
	return success;
}

void Grammar::set_start(Symbol start_symbol)
{
	if (start_symbol.type() != Symbol::no_terminal) [[unlikely]]
		throw std::logic_error { "start symbol must specifie no_terminal" };
	if (!m_rules.contains(start_symbol)) [[unlikely]]
		throw std::logic_error { "unkown start symbol" };

	m_start_symbol.emplace(std::move(start_symbol));
}

void Grammar::display_latex(std::ostream& os, bool markdown) const
{
	if (markdown) std::println(os, "$$");
	
	std::println(os, R"(\begin{{array}}{{rcl}})");
	for (const auto& [lhs, set]: m_rules)
	{
		
		std::print(os, R"(\langle \text{{{}}} \rangle & \rightarrow & )", lhs.symbol_string());
		for (auto itr = set.cbegin();
			 itr != set.cend();
			 ++itr)
		{
			const auto& rhs = *itr;
			if (rhs.empty())
			{
				std::print(os, R"(ε )");
			}

			for (const auto& rhs_value : rhs)
			{
				if (rhs_value.type() == Symbol::no_terminal)
					std::print(os, R"(\langle \text{{{}}} \rangle )", rhs_value.symbol_string());
				else if (rhs_value.type() == Symbol::terminal)
					std::print(os, R"({} )", rhs_value.symbol_string());
			}

			if (std::next(itr) != set.cend())
				std::print(os, R"(\ | \ )") ;
		}
		std::println(os, R"(\\)") ;
	}

	std::println(os, R"(\end{{array}})");
	if (markdown) std::println(os, "$$");
}

bool Grammar::validate_noterminals() const
{
	std::unordered_map<Symbol, bool> uniq_noterminal;
	if (!m_start_symbol.has_value())
		return false;

	uniq_noterminal.emplace(m_start_symbol.value(), false);

	for (const auto& [left, right_set] : m_rules)
	{
		auto left_itr = uniq_noterminal.find(left);
		if (left_itr == uniq_noterminal.end())
			uniq_noterminal.emplace(left, true);
		else
			left_itr->second = true;
		
		for (const auto& right : right_set)
		{
			std::ranges::for_each(right,[&](const auto& symbol) {
				//如果存在则会插入失败，不影响原值
				uniq_noterminal.emplace(symbol, false);
			});
		}
	}

	for (const auto& [_, is_defined] : uniq_noterminal)
		if (!is_defined)
			return false;
	return true;
}

[[nodiscard]]
bool Grammar::nullable(const Symbol& symbol) const
{
	if (m_nullable.empty())
		throw std::logic_error { "nullable table uninitialized" };

	if (symbol.type() != Symbol::no_terminal)
		return false;
	
	return m_nullable.at(symbol);
}

void Grammar::infer_empty_string()
{
	for (const auto& [left, right_set] : m_rules)
	{
		m_nullable.emplace(left, false);
		for (const auto& right : right_set)
		{
			if (right.size() > 1)
				continue;

			if (right.empty())
				throw std::logic_error { "invalid right rule" };
		}
	}
	
}

bool Grammar::infer_empty_string(const Symbol& symbol)
{
	
}

tl::expected<Grammar, std::string> YamlParser::parse(std::istream& in)
{
	auto filed_error_msg = [](std::string_view sv)
	{
		return std::format("The yaml file {} is missing [terminal] filed", sv.data());
	};

	YAML::Node rules = YAML::Load(in);
	
	//move
	auto terminals = rules["terminals"];
	if (!terminals.IsDefined())	[[unlikely]]
	{
		return tl::make_unexpected(filed_error_msg("terminals"));
	}
	
	std::unordered_set<std::string> terminal_string_set;
	for (auto itr = terminals.begin();
		 itr != terminals.end();
		 ++itr)
	{
		if (auto terminal_ret = nothrow_as<std::string>(static_cast<YAML::Node>(*itr));
			terminal_ret) [[likely]]
			terminal_string_set.insert(*terminal_ret);
		else
			return tl::make_unexpected(terminal_ret.error());
	}
	
	//result
	Grammar grammar;

	auto start_symbol = rules["start_symbol"];
	if (start_symbol.IsDefined())
	{
		std::string symbol; 
		if (auto start_ret = nothrow_as<std::string>(start_symbol);
			start_ret) [[likely]]
			symbol = *start_ret;
		else
			return tl::make_unexpected(start_ret.error());
		grammar.set_start(Symbol{ terminal_string_set.contains(symbol), symbol });
	}
	
	auto productions = rules["productions"];
	if (!productions.IsDefined()) [[unlikely]]
		return tl::make_unexpected(filed_error_msg("productions"));
	
	if (productions.IsNull()) [[unlikely]]
		return tl::make_unexpected("productions is null");

	bool first_value_mark = true;
	for (auto rule: productions)
	{
		std::string lhs_string;
		if (auto lhs_ret = nothrow_as<std::string>(rule.first);
			lhs_ret) [[likely]]
			lhs_string = *lhs_ret;
		else
			return tl::make_unexpected(lhs_ret.error());
		
		//文法产生式左部必须为非终结符号
		if (terminal_string_set.contains(lhs_string)) [[unlikely]]
			return tl::make_unexpected(
				std::format("Left can't {} be terminal value", lhs_string)
			);

		Symbol left { Symbol::no_terminal, lhs_string };
		
		//如果yaml没有指定开始标记，则以产生式集的第一个为开始符号
		if (first_value_mark && !productions.IsDefined()) [[unlikely]]
		{
			first_value_mark = false;
			grammar.set_start(Symbol{ Symbol::no_terminal, lhs_string });
		}
		
		parse_rhs(left, grammar, rule.second, terminal_string_set);
	}

	return grammar;
}

template<typename Ty>
tl::expected<Ty, std::string> YamlParser::nothrow_as(const YAML::Node& node)
{
	try {
		return node.as<Ty>();
	} catch(const YAML::Exception& e) {
		return tl::make_unexpected(e.msg);
	} catch(const std::exception& e) {
		return tl::make_unexpected(e.what());
	} catch(...) {
		return tl::make_unexpected("Unkown expection when parsing terminal filed");
	}
}

auto YamlParser::parse_rhs(Symbol& left, Grammar& grammar, const YAML::Node& rhs_set,
			   	 		   const std::unordered_set<std::string>& terminal_string_set) const
	-> tl::expected<void, std::string>
{
	std::vector<Symbol> right;
	for (auto right_node : rhs_set)
	{
		right.clear();
		right.reserve(right_node.size());
		for (auto str_node: right_node)
		{
			if (auto str_node_ret = nothrow_as<std::string>(str_node);	
				str_node_ret) [[likely]]
			{
				bool terminal_flag = terminal_string_set.contains(*str_node_ret);
				right.emplace_back(terminal_flag, *str_node_ret);
			}
			else [[unlikely]]
			{
				return tl::make_unexpected(str_node_ret.error());
			}
		}
		// right包含空串
		if (right.empty())
			right.push_back(Grammar::empty_symbol);

		if (!grammar.add_rule(left, std::move(right)))	[[unlikely]]
		{
			YAML::Emitter out;
			out << rhs_set;
			return tl::make_unexpected(
				std::format("add_rules failed: the same rule [{}] is difined twice", out.c_str())
			);
		}
	}

	return {};
}


}	//namespace analyze

