#pragma once
#include <system_error>
#include <iostream>
#include <fstream>

namespace yq
{

enum class stream_errc
{
    success = 0,
    eof,
    open_failed,
    read_failed,
    write_failed,
    invalid_format,
    invalid_file_path,
    critical_error,
};

class stream_error_category : public ::std::error_category
{
public:
	const char* name() const noexcept override
	{ return "stream_error"; }
	
	std::string message(int ev) const override
	{
		switch(static_cast<stream_errc>(ev))
		{
		case stream_errc::success:
			return "Success";
		case stream_errc::open_failed:
			return "Failed to open stream";
		case stream_errc::read_failed:
			return "Failed to read from stream";
		case stream_errc::write_failed:
			return "Failed to write to stream";
		case stream_errc::invalid_format:
			return  "Invalid data format";
		case stream_errc::eof:
			return "End of file reached";
		case stream_errc::invalid_file_path:
			return "Invalid file path";
		case stream_errc::critical_error:
			return "Critical io error";
		default:
			return "Unknown stream error";

		}
	}
};

[[nodiscard]]
const ::std::error_category& get_stream_error_category();

[[nodiscard]]
::std::error_code make_error_code(stream_errc e);

[[nodiscard]]
::std::error_code get_stream_error(::std::ios& stream);

[[nodiscard]]
::std::error_code get_stream_error(::std::istream& istream);

[[nodiscard]]
::std::error_code get_stream_error(::std::ostream& ostream);

[[nodiscard]]
::std::error_code get_stream_error(::std::ifstream& infile);

}	//namespace yq

namespace std
{
template <>
struct is_error_code_enum<::yq::stream_errc> : true_type {};
}	//namespace std	

