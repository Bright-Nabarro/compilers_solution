#include <format>
#include "configure.hpp"

namespace simple_regex
{
void MainConfigure::set_printable(bool mark)
{
	m_printable = mark;
}

void MainConfigure::set_outputdir(std::string_view sv)
{
	m_outputdir = sv.data();
}

auto MainConfigure::set_model(std::string_view sv) -> tl::expected<void, std::string>
{
	if (sv == "DFA")
		m_modelId = ModelId::DFA;
	else if (sv == "DFAE")
		m_modelId = ModelId::DFAE;
	else if (sv == "NFA")
		m_modelId = ModelId::NFA;
	else
		return tl::make_unexpected(std::format("expected model name {}", sv.data()));
	return {};
}

auto MainConfigure::set_regex_string(std::string_view sv) -> void
{
	m_regexString = sv.data();
}

auto MainConfigure::set_istream(std::string_view sv) -> tl::expected<void, std::string>
{
	m_file.open(sv.data(), std::ios::ate);
	if (!m_file)
		return tl::make_unexpected( std::format("cannot open {}", sv.data()) );
	m_inStream = &m_file;

	return {};
}

auto MainConfigure::set_allfileMark(bool mark) -> void
{
	m_allfileMark = mark;
}

auto MainConfigure::set_noparse(bool mark) -> void
{
	m_noparse = mark;
}

auto MainConfigure::get_printable() const -> bool
{
	return m_printable;
}

auto MainConfigure::get_outputdir() const -> std::string
{
	return m_outputdir;
}

auto MainConfigure::get_model() const -> ModelId
{
	return m_modelId;
}

auto MainConfigure::get_regex_string() const -> std::string
{
	return m_regexString;
}

auto MainConfigure::get_istream() const -> std::istream&
{
	return *m_inStream;
}

auto MainConfigure::allfile() const -> bool
{
	return m_allfileMark;
}

auto MainConfigure::noparse() const -> bool
{
	return m_noparse;
}

}	//namespace simple_regex
