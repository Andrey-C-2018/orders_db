#include "Cryptor.h"

CryptorException::CryptorException(const int err_code, const Tchar *err_descr) : \
									XException(err_code, err_descr) { }

CryptorException::CryptorException(const CryptorException &obj) : XException(obj) { }

CryptorException::~CryptorException() { }

//*****************************************************

enum Constants {
	RANGES_COUNT = 3, \
	CHARS_SET_LEN = 11
};

struct CharsRange {
	int id;
	Tchar begin;
	Tchar end;
};

CharsRange ranges[RANGES_COUNT] = { { 1, '0', '9' },{ 2, 'a', 'z' }, \
														{3, 'A', 'Z'} };
size_t ranges_chars_len = 0;
const Tchar chars_set[CHARS_SET_LEN] = _T(",.#$%^&*_+");

inline size_t getCharUpperLimit() {

	assert(ranges_chars_len);
	return ranges_chars_len + sizeof(chars_set) / sizeof(Tchar) - 1;
}

inline size_t getRangeSize(const size_t range_index) {

	assert(range_index < RANGES_COUNT);
	return ranges[range_index].end - ranges[range_index].begin + 1;
}

inline void calcRangesCharsLen() {

	assert(ranges_chars_len == 0);
	for (size_t i = 0; i < RANGES_COUNT; ++i) {
		assert(ranges[i].begin < ranges[i].end);
		ranges_chars_len += getRangeSize(i);
	}
}

inline short charToIndex(const Tchar ch) {

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

inline Tchar indexToChar(const short index) {

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

void Cryptor::init (const unsigned char *key_, size_t key_len) {

	if (ranges_chars_len == 0) calcRangesCharsLen();

	key.reserve(key_len);
	for (size_t i = 0; i < key_len; ++i)
		key.push_back(key_[i] % getCharUpperLimit());
}

Tstring Cryptor::encrypt(const Tchar *cler) const {

	assert(cler);
	const size_t len = Tstrlen(cler);
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
