#pragma once
#include <string_view>
#include <string>
#include <tl/expected.hpp>
#include <fstream>
#include <iostream>

namespace simple_regex
{

enum class ModelId
{
	DFA,
	DFAE,
	NFA
};

class MainConfigure
{
	
public:
	MainConfigure():
		m_printable { false },
		m_outputdir { "." },
		m_modelId { ModelId::DFA },
		m_regexString {},
		m_inStream {&std::cin},
		m_file {},
		m_allfileMark { false },
		m_noparse { false }
	{
	}

	auto set_printable(bool mark) -> void;
	auto set_outputdir(std::string_view sv) -> void;
	auto set_model(std::string_view sv) -> tl::expected<void, std::string>;
	auto set_regex_string(std::string_view sv) -> void;
	auto set_istream(std::string_view sv) -> tl::expected<void, std::string>;
	auto set_allfileMark(bool mark) -> void;
	auto set_noparse(bool mark) -> void;

	auto get_printable() const -> bool;
	auto get_outputdir() const -> std::string;
	auto get_model() const -> ModelId;
	auto get_regex_string() const -> std::string;
	auto get_istream() const -> std::istream&;
	auto allfile() const -> bool;
	auto noparse() const -> bool;

private:
	bool m_printable;
	std::string m_outputdir;
	ModelId m_modelId;
	std::string m_regexString;
	std::istream* m_inStream;
	std::ifstream m_file;
	bool m_allfileMark;
	bool m_noparse;
};

}
