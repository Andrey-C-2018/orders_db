#include "Cryptor.h"

CryptorException::CryptorException(const int err_code, const Tchar *err_descr) : \
									XException(err_code, err_descr) { }

CryptorException::CryptorException(const CryptorException &obj) : XException(obj) { }

CryptorException::~CryptorException() { }

//*****************************************************

Cryptor::CharsRange Cryptor::ranges[RANGES_COUNT] = { { 1, '0', '9' },{ 2, 'a', 'z' }, \
														{3, 'A', 'Z'} };
size_t Cryptor::ranges_chars_len = 0;
const Tchar Cryptor::chars_set[CHARS_SET_LEN] = L",.#$%^&*_+";

Tstring Cryptor::encrypt(const Tchar *cler) const {

	assert(cler);
	const size_t len = wcslen(cler);
	if (len > key.size())
		throw CryptorException(CryptorException::E_MESSAGE_TOO_BIG, \
							_T("the message length is bigger than the key length"));
	
	Tstring encryped;
	encryped.reserve(len);

	for (size_t i = 0; i < len; ++i) {
		const Tchar open_ch = cler[i];

		short index = charToIndex(open_ch);
		index ^= key[i];
		encryped += indexToChar(index);
	}

	return encryped;
}

Cryptor::~Cryptor() { }
