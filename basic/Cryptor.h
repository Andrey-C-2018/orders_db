#pragma once
#include <vector>
#include "Exception.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

class BASIC_DLL_EXPORT CryptorException : public XException {
	Tstring prop_name;
public:
	enum Errors {
		E_ILLEGAL_CHARACTER = 1, \
		E_MESSAGE_TOO_BIG = 2
	};
	CryptorException(const int err_code, const Tchar *err_descr);
	CryptorException(const CryptorException &obj);
	CryptorException(CryptorException &&obj) = default;

	~CryptorException() throw();
};

class BASIC_DLL_EXPORT Cryptor {

	std::vector<unsigned char> key;

	void init(const unsigned char *key, size_t key_len);

public:
	template <size_t N> \
		Cryptor(const unsigned char(&key_)[N]);

	Cryptor(const Cryptor &obj) = default;
	Cryptor(Cryptor &&obj) = default;
	Cryptor &operator=(const Cryptor &obj) = default;
	Cryptor &operator=(Cryptor &&obj) = default;

	Tstring encrypt(const Tchar *cler) const;

	virtual ~Cryptor();
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

//*****************************************************

template <size_t N> \
Cryptor::Cryptor(const unsigned char(&key_)[N]) {

	init(key_, N);
}
