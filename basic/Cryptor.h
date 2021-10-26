#pragma once
#include <vector>
#include "Exception.h"

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

class Cryptor {
	enum Constants{
		RANGES_COUNT = 3, \
		CHARS_SET_LEN = 11
	};

	struct CharsRange {
		int id;
		Tchar begin;
		Tchar end;
	};

	static CharsRange ranges[RANGES_COUNT];
	static size_t ranges_chars_len;
	static const Tchar chars_set[CHARS_SET_LEN];

	std::vector<unsigned char> key;

	inline size_t getCharUpperLimit() const;
	inline size_t getRangeSize(const size_t range_index) const;
	inline void calcRangesCharsLen() const;
	inline short charToIndex(const Tchar ch) const;
	inline Tchar indexToChar(const short index) const;

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

//*****************************************************

template <size_t N> \
Cryptor::Cryptor(const unsigned char(&key_)[N]) {

	if (ranges_chars_len == 0) calcRangesCharsLen();

	key.reserve(N);
	for (size_t i = 0; i < N; ++i)
		key.push_back(key_[i] % getCharUpperLimit());
}

size_t Cryptor::getCharUpperLimit() const {

	assert(ranges_chars_len);
	return ranges_chars_len + sizeof(chars_set) / sizeof(Tchar) - 1;
}

size_t Cryptor::getRangeSize(const size_t range_index) const {

	assert(range_index < RANGES_COUNT);
	return ranges[range_index].end - ranges[range_index].begin + 1;
}

void Cryptor::calcRangesCharsLen() const {

	assert(ranges_chars_len == 0);
	for (size_t i = 0; i < RANGES_COUNT; ++i) {
		assert(ranges[i].begin < ranges[i].end);
		ranges_chars_len += getRangeSize(i);
	}
}

short Cryptor::charToIndex(const Tchar ch) const {

	auto p = Tstrchr(chars_set, ch);
	if (p != nullptr)
		return (short)(ranges_chars_len + p - chars_set);

	bool found = false;
	size_t i = 0;
	size_t sizes_summ = 0;
	while (!found && i < RANGES_COUNT) {
		found = ranges[i].begin <= ch && ch <= ranges[i].end;
		sizes_summ += getRangeSize(i);
		++i;
	}

	if (!found) {
		CryptorException e(CryptorException::E_ILLEGAL_CHARACTER, \
							_T("illegal character: "));
		e << ch;
		throw e;
	}
	--i;
	return (short)(sizes_summ - ranges[i].end + ch - 1);
}

Tchar Cryptor::indexToChar(const short index) const {

	assert(index >= 0 && index <= getCharUpperLimit());

	if (index < ranges_chars_len) {

		size_t i = 0;
		size_t Index = index;
		while (Index >= getRangeSize(i)) {
			assert(i < RANGES_COUNT);
			Index -= getRangeSize(i);
			++i;
		}

		return ranges[i].begin + (Tchar)Index;
	}

	return chars_set[index - ranges_chars_len];
}
