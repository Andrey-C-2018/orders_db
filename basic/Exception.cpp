#include "Exception.h"
#include "XConv.h"

template <typename Tchar_, class Tstring_> \
inline void Append(const Tchar_ *str, Tstring_ &buffer) {

	assert(str);
	buffer += str;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

inline void Tstrconv(char *Dest, const wchar_t *Src, size_t MaxCount) { wcstombs(Dest, Src, MaxCount); }
inline void Tstrconv(wchar_t *Dest, const char *Src, size_t MaxCount) { mbstowcs(Dest, Src, MaxCount); }

#ifdef _MSC_VER
#pragma warning(pop)
#endif

template <typename Tchar_, class Tstring_> \
inline void ConvertAndAppend(const Tchar_ *str, Tstring_ &buffer, const size_t size) {

	static_assert(!std::is_same<Tchar_, typename Tstring_::value_type>::value, \
					"The arguments must be of different str types");
	assert(str);

	size_t old_size = buffer.size();
	buffer.resize(old_size + size);

	Tstrconv(&buffer[old_size], str, size);
}

inline void ConvertionWrapper(const char *str, std::string &buffer) { Append(str, buffer); }

inline void ConvertionWrapper(const char *str, std::wstring &buffer) { ConvertAndAppend(str, buffer, strlen(str)); }

inline void ConvertionWrapper(const wchar_t *str, std::wstring &buffer) { Append(str, buffer); }

inline void ConvertionWrapper(const wchar_t *str, std::string &buffer) { ConvertAndAppend(str, buffer, wcslen(str)); }

inline void ConvertionWrapper(const std::string &str, std::string &buffer) { Append(str.c_str(), buffer); }

inline void ConvertionWrapper(const std::string &str, std::wstring &buffer) { ConvertAndAppend(str.c_str(), buffer, str.size()); }

inline void ConvertionWrapper(const std::wstring &str, std::wstring &buffer) { Append(str.c_str(), buffer); }

inline void ConvertionWrapper(const std::wstring &str, std::string &buffer) { ConvertAndAppend(str.c_str(), buffer, str.size()); }

//**************************************************************

XException::XException(const int err_code, const char *err_descr){

	ConvertionWrapper(err_descr, this->err_msg);
	this->err_code = err_code;
}

XException::XException(const int err_code, const wchar_t *err_descr) {

	ConvertionWrapper(err_descr, this->err_msg);
	this->err_code = err_code;
}

XException::XException(const int err_code, const std::string &err_descr){

	ConvertionWrapper(err_descr, this->err_msg);
	this->err_code = err_code;
}

XException::XException(const int err_code, const std::wstring &err_descr) {

	ConvertionWrapper(err_descr, this->err_msg);
	this->err_code = err_code;
}

XException::XException(const int err_code, const size_t reserved_size){

	err_msg.reserve(reserved_size);
	this->err_code = err_code;
}

XException::XException(const XException &obj) : err_code(obj.err_code),\
												err_msg(obj.err_msg){ }

XException &XException::operator<<(const char *new_data) {

	ConvertionWrapper(new_data, this->err_msg);
	return *this;
}

XException &XException::operator<<(const wchar_t *new_data) {

	ConvertionWrapper(new_data, this->err_msg);
	return *this;
}

XException &XException::operator<<(const std::string &new_data) {

	ConvertionWrapper(new_data, this->err_msg);
	return *this;
}

XException &XException::operator<<(const std::wstring &new_data) {

	ConvertionWrapper(new_data, this->err_msg);
	return *this;
}

XException &XException::operator<<(const int number) {
	Tchar buffer[getDigitsCountOfType<int>() + 1];

	err_msg.append(XConv::ToString(number, buffer));
	return *this;
}

#if _WIN64
XException &XException::operator<<(const unsigned number) {
	Tchar buffer[getDigitsCountOfType<unsigned>() + 1];

	err_msg.append(XConv::ToString(number, buffer));
	return *this;
}
#endif

XException &XException::operator<<(const size_t number) {
	Tchar buffer[getDigitsCountOfType<size_t>() + 1];

	err_msg.append(XConv::ToString(number, buffer));
	return *this;
}

const Tchar* XException::what() const noexcept {

	return err_msg.c_str();
}

int XException::GetErrorCode() const noexcept {
	
	return err_code;
}

XException::~XException(){ }
