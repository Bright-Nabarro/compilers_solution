#include "stream_error.hpp"

namespace yq
{

const ::std::error_category& get_stream_error_category()
{
	static stream_error_category instance;
	return instance;
}

[[nodiscard]]
::std::error_code make_error_code(stream_errc e)
{
	return { static_cast<int>(e), get_stream_error_category() };
}

[[nodiscard]]
::std::error_code get_stream_error(::std::ios& stream)
{
	//if (stream.eof())
	//	return make_error_code(stream_errc::eof);
	//if (stream.bad())
	//	return make_error_code(stream_errc::read_write_faild);
	//if (stream.fail())
	//	return make_error_code(stream_errc::invalid_format);
	//	
	//return make_error_code(stream_errc::success);
	auto ios_state = stream.rdstate();

	if (ios_state & std::ios::badbit)
		return make_error_code(stream_errc::critical_error);
	if (ios_state & std::ios::failbit)
	{
		if (ios_state & std::ios::eofbit)
			return make_error_code(stream_errc::eof);
		else
			return make_error_code(stream_errc::invalid_format);
	}
	if (ios_state & std::ios::eofbit)
		return make_error_code(stream_errc::eof);
	
	return make_error_code(stream_errc::success);
}

[[nodiscard]]
::std::error_code get_stream_error(::std::istream& istream)
{
	if (istream.bad())
		return make_error_code(stream_errc::read_failed);
	else
		return get_stream_error(static_cast<::std::ios&>(istream));
}

[[nodiscard]]
::std::error_code get_stream_error(::std::ostream& ostream)
{
	if (ostream.bad())
		return make_error_code(stream_errc::write_failed);
	else
		return get_stream_error(static_cast<::std::ios&>(ostream));

}

[[nodiscard]]
::std::error_code get_stream_error(::std::ifstream& infile)
{
	if (infile.bad())
		return make_error_code(stream_errc::invalid_file_path);
	else
		return get_stream_error(static_cast<::std::istream&>(infile));
}

}	//namespace yq
