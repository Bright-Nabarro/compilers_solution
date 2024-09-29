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
	auto ios_state = stream.rdstate();

	if (ios_state & std::ios::badbit)
		return make_error_code(stream_errc::critical_error);
	if (ios_state & std::ios::eofbit)
		return make_error_code(stream_errc::eof);
	if (ios_state & std::ios::failbit)
		return make_error_code(stream_errc::invalid_format);

	return make_error_code(stream_errc::success);
}

[[nodiscard]]
::std::error_code get_stream_error(::std::istream& istream)
{
	auto ret = get_stream_error(static_cast<::std::ios&>(istream));

	if (ret == make_error_code(stream_errc::critical_error))
		return make_error_code(stream_errc::read_failed);

	return ret;
}

[[nodiscard]]
::std::error_code get_stream_error(::std::ostream& ostream)
{
	auto ret = get_stream_error(static_cast<::std::ios&>(ostream));

	if (ret == make_error_code(stream_errc::critical_error))
		return make_error_code(stream_errc::write_failed);

	return ret;
}

}	//namespace yq

