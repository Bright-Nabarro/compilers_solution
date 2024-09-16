#include <print>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <fstream>
#include "configure.hpp"
#include "syntax_tree.hpp"
#include "dfa.hpp"
#include "parse.hpp"

namespace po = boost::program_options;
namespace sr = simple_regex;

//如果返回false, 代表非错误退出
auto parse_main_arg(sr::MainConfigure& mainConfigure,
		int argc, char* argv[]) -> tl::expected<bool, std::string>
{
	po::options_description desc { "Options:" };
	desc.add_options()
		("help,h", "Print this help message")
		("model,m", po::value<std::string>(),
		 	"Choose model DFA, DFAE or NFA (DFA is default, DFAE add dead cvt")
		("outdir,d", po::value<std::string>(), "Specifize output directory")
		("print,p", "Enable output tree.md and graph.md")
		("regex,r", po::value<std::string>(), "Input Regex")
		("input,I", po::value<std::string>(), "Input filename")
		("all,A", "parse all file ( default each line )");
	
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		vm.notify();
	} catch(const po::error& ex) {
		std::println(std::cerr, "{}", ex.what());
	}

	if (vm.count("help"))
	{
		desc.print(std::cout);
		return false;
	}
	
	if (vm.count("model"))
	{
		auto ret = mainConfigure.set_model(vm["model"].as<std::string>());
		if (!ret)
		{
			return tl::make_unexpected(ret.error());
		}
	}
	
	if (vm.count("outdir"))
	{
		mainConfigure.set_outputdir(vm["outdir"].as<std::string>());
	}
	
	if (vm.count("print"))
	{
		mainConfigure.set_printable(true);
	}
	
	if (vm.count("regex"))
	{
		mainConfigure.set_regex_string(vm["regex"].as<std::string>());
	}
	else
	{
		return tl::make_unexpected("regex expression is not spectialized");
	}

	if (vm.count("input"))
	{
		mainConfigure.set_istream(vm["input"].as<std::string>());
	}

	if (vm.count("all"))
	{
		mainConfigure.set_allfileMark(true);
	}

	return true;
}

template<typename ObjFunc, typename Obj>
auto state_display(const sr::MainConfigure& mainConfigure,
	ObjFunc objfunc, const Obj& obj) -> tl::expected<void, std::string>
{
	using namespace std::string_literals;
	if (!mainConfigure.get_printable())
		return {};

	auto filePath = mainConfigure.get_outputdir() + "/"s + "output.md"s; 
	static std::ofstream outfile {filePath, std::ios::out};
	if (!outfile)
	{
		return tl::make_unexpected(
				std::format("ofstream error: cannot open {}", filePath)
		);
	}
	(obj.*objfunc)(outfile);

	return {};
}

auto main_loop(const sr::MainConfigure& mainConfigure)
	-> tl::expected<void, std::string>
{
	sr::SyntaxTree tree;
	if (mainConfigure.get_model() != sr::ModelId::DFA)
	{
		return tl::make_unexpected(std::format("ModelId {} not support yet", 
			static_cast<int>(mainConfigure.get_model())));
	}
	
	auto treeRet = tree.parse_regex(mainConfigure.get_regex_string());
	if (!treeRet)
	{
		return
			tl::make_unexpected(std::format("regex error: {}", treeRet.error()));
	}
	
	//auto displayRet = state_display(mainConfigure,
	//	static_cast<void (sr::SyntaxTree::*)(std::ostream& os) const>
	//	(&sr::SyntaxTree::display), tree);
	//if (!displayRet)
	//{
	//	std::println(std::cerr, "display error: {}", displayRet.error());
	//}
	
	sr::DFA dfa;
	dfa.create_graph(std::move(tree));

	auto displayRet = state_display(mainConfigure, &sr::DFA::display_graph, dfa);
	if (!displayRet)
	{
		std::println(std::cerr, "display error: {}", displayRet.error());
	}

	simple_regex::Parse parseContext { std::move(dfa) };
	
	auto praseStreamRet = parseContext.parse_stream(mainConfigure.get_istream());
	if (!praseStreamRet)
		return praseStreamRet;
	
	decltype(auto) parseStringList = parseContext.get_parse_string_list();
	
	if (parseStringList.empty())
		std::println(stdout, "miss match");

	for (decltype(auto) parseString :parseStringList)
	{
		std::println(stdout, "parse string[{}]:\t{}",
				parseString.begPos, parseString.context);
	}

	return {};
}

int main(int argc, char* argv[])
{
	sr::MainConfigure mainConfigure;
	auto parseRet = parse_main_arg(mainConfigure, argc, argv);
	if (!parseRet)
	{
		std::println(std::cerr, "Initail error: {}", parseRet.error());
		return 1;
	}
	if (!*parseRet)
		return 0;
	
	auto mainLoopRet = main_loop(mainConfigure);
	if (!mainLoopRet)
	{
		std::println("{}", mainLoopRet.error());
		return 1;
	}

	return 0;
}

