#include <UnitTest++/UnitTest++.h>
#include <basic/XString.h>
#include <algorithm>

const size_t MAX_STR_SIZE = 40;
const size_t test_arr_size = 10;
const Tchar test_strings[test_arr_size][MAX_STR_SIZE] = \
							{ _T(""), _T("test"), \
								_T("0"), _T("io"), \
								_T("12345678890abcdefghijklmopqrstvwxyz"), \
								_T("abc"), _T("_*_"), _T("901"), _T(",."), _T(",,") };

const Tchar test_chars[] = { _T('\0'), _T('0'), _T('\r'), _T('\t'), \
							_T('a'), _T('\n'), _T('A'), _T('*') };

bool IsEmpty(const XString<Tchar> &str) {

	const bool empty = str.empty();
	const bool contains_null_str = (str.c_str() == nullptr);
	const bool contains_empty_str = !contains_null_str && (str.c_str()[0] == 0);

	CHECK(!contains_null_str || (contains_null_str && contains_null_str == empty));
	CHECK(!contains_empty_str || (contains_empty_str && contains_empty_str == empty));

	return empty;
}

bool IsEqual(const XString<Tchar> &str1, const XString<Tchar> &str2) {

	const bool equal = str1 == str2;
	const bool contains_empty_str1 = str1.c_str() && str1.c_str()[0] == 0;
	const bool contains_empty_str2 = str2.c_str() && str2.c_str()[0] == 0;
	const bool empty1 = IsEmpty(str1);
	const bool empty2 = IsEmpty(str2);
	
	if ((!empty1 && !empty2) || \
		(contains_empty_str1 && contains_empty_str2) || \
		(contains_empty_str1 && !empty2) || \
		(!empty1 && contains_empty_str2)){

		bool equal_thru_strcmp = !Tstrcmp(str2.c_str(), str1.c_str());
		CHECK(equal == equal_thru_strcmp);
	}

	return equal;
}

bool IsEqual(const XString<Tchar> &str1, const Tchar *str2) {

	const bool equal = str1 == str2;
	const bool contains_empty_str1 = str1.c_str() && str1.c_str()[0] == 0;
	const bool contains_empty_str2 = str2 && str2[0] == 0;
	const bool empty1 = IsEmpty(str1);
	const bool empty2 = !str2 || (str2 && str2[0] == 0);

	if ((!empty1 && !empty2) || \
		(contains_empty_str1 && contains_empty_str2) || \
		(contains_empty_str1 && !empty2) || \
		(!empty1 && contains_empty_str2)) {

		bool equal_thru_strcmp = !Tstrcmp(str2, str1.c_str());
		CHECK(equal == equal_thru_strcmp);
	}

	return equal;
}

SUITE(XString_tests) {

	TEST(Initializing_def_ctor) {
		XString<Tchar> str;

		CHECK(IsEmpty(str));
	}

	TEST(Initializing_copy_ctor) {

		for (size_t i = 0; i < test_arr_size; ++i) {
			XString<Tchar> str1(test_strings[i]);
			XString<Tchar> str2(str1);

			CHECK_EQUAL(str2.size(), str1.size());
			CHECK((str2.empty() && str1.empty()) || \
				(!(str2.empty() && str1.empty()) && \
					str2.c_str() != str1.c_str())); // mutual buffer is forbidden
			CHECK(IsEqual(str1, str2));
		}
	}

	TEST(Initializing_Cstr_ctor) {

		for (size_t i = 0; i < test_arr_size; ++i) {
			XString<Tchar> str(test_strings[i]);

			CHECK_EQUAL(Tstrlen(test_strings[i]), str.size());
			CHECK(str.c_str() != test_strings[i]); // a copy should be done
			CHECK(IsEqual(str, test_strings[i]));
		}
	}

	TEST(Initializing_Char_ctor) {

		XString<Tchar> str;
		size_t j = 0;
		for (size_t i = 0; i < sizeof(test_chars) / sizeof(Tchar); ++i) {
			XString<Tchar> str1(test_chars[i]);

			if (test_chars[i] != 0) {
				CHECK_EQUAL(1, str1.size());
				CHECK_EQUAL(test_chars[i], *(str1.c_str()));
			}
			else
				CHECK_EQUAL(0, str1.size());
			
			str = test_strings[j];
			str = test_chars[i];

			if (test_chars[i] != 0) {
				CHECK_EQUAL(1, str.size());
				CHECK_EQUAL(test_chars[i], *(str.c_str()));
			}
			else
				CHECK_EQUAL(0, str.size());

			++j;
			j *= (j < test_arr_size);
		}
	}

	TEST(Copying) {

		XString<Tchar> str;
		for (size_t i = 0; i < test_arr_size; ++i) {
			XString<Tchar> str1;
			XString<Tchar> str2 = test_strings[i];

			str = str2;
			CHECK_EQUAL(str.size(), str2.size());
			CHECK(str2.c_str() != test_strings[i]); // a copy should be done
			CHECK(IsEqual(str2, test_strings[i]));
			CHECK((str.empty() && str2.empty()) || \
				(!(str.empty() && str2.empty()) && \
					str.c_str() != str2.c_str())); // mutual buffer is forbidden
			CHECK(IsEqual(str, str2));

			str1 = str.c_str();
			CHECK_EQUAL(str1.size(), str.size());
			CHECK((str1.empty() && str.empty()) || \
				(!(str1.empty() && str.empty()) && \
					str1.c_str() != str.c_str())); // mutual buffer is forbidden
			CHECK(IsEqual(str1, str));
		}
	}

	TEST(Moving) {

		XString<Tchar> str;
		for (size_t i = 0; i < test_arr_size; ++i) {
			XString<Tchar> str2 = test_strings[i];
			XString<Tchar> str3 = std::move(XString<Tchar>(test_strings[test_arr_size - i - 1]));
			XString<Tchar> str4 = XString<Tchar>(test_strings[test_arr_size - i - 1]);

			size_t test_str_sz = Tstrlen(test_strings[test_arr_size - i - 1]);
			CHECK_EQUAL(str3.size(), str4.size());
			CHECK_EQUAL(test_str_sz, str4.size());

			CHECK((str3.empty() && str4.empty()) || \
				(!(str3.empty() && str4.empty()) && \
					str3.c_str() != str4.c_str())); // can't share the same buffer
			CHECK(IsEqual(str3, test_strings[test_arr_size - i - 1]));
			CHECK(IsEqual(str3, str4));

			str = std::move(str2);
			CHECK_EQUAL(Tstrlen(test_strings[i]), str.size());
			CHECK(IsEqual(str, test_strings[i]));
			CHECK(str2.empty());
		}
	}

	TEST(Append_CStr) {

		Tstring etalon;
		XString<Tchar> str;
		size_t sizes_summ = 0;
		for (size_t i = 0; i < test_arr_size; ++i) {
			XString<Tchar> str2;
			size_t test_str_sz = Tstrlen(test_strings[i]);

			str2 += XString<Tchar>(test_strings[i]);
			CHECK_EQUAL(test_str_sz, str2.size());
			CHECK(IsEqual(str2, test_strings[i]));
			str += str2;

			etalon += test_strings[i];

			sizes_summ += test_str_sz;
		}

		CHECK_EQUAL(sizes_summ, str.size());
		CHECK_EQUAL(etalon.size(), str.size());
		CHECK_EQUAL(etalon.size(), sizes_summ);
		CHECK(IsEqual(str, etalon.c_str()));
	}

	TEST(Append_char) {

		for (size_t j = 0; j < test_arr_size; ++j) {
			XString<Tchar> str;
			const size_t curr_str_size = Tstrlen(test_strings[j]);

			for (size_t i = 0; i < curr_str_size; ++i) {
				XString<Tchar> str1;

				str1 += test_strings[j][i];
				if (test_strings[j][i] != 0) {
					CHECK_EQUAL(1, str1.size());
					CHECK_EQUAL(test_strings[j][i], *(str1.c_str()));
				}
				else
					CHECK_EQUAL(0, str1.size());

				str += str1;
			}

			if (test_strings[j][0] != 0)
				CHECK_EQUAL(curr_str_size, str.size());
			else
				CHECK(IsEmpty(str));

			CHECK(IsEqual(str, test_strings[j]));
		}
	}

	TEST(Compare_Equal) {
		Tchar blank[] = { 0, 0, 0, 0, 0 };
		Tchar blank2[] = { 0, 'a', 0, 0 };

		CHECK(XString<Tchar>(_T("")) == XString<Tchar>(_T("")));
		CHECK(XString<Tchar>(_T("")) == XString<Tchar>(blank));
		CHECK(XString<Tchar>(_T("")) == XString<Tchar>(blank2));

		XString<Tchar> str1;
		CHECK(XString<Tchar>(_T("")) == str1);
		CHECK(str1 == XString<Tchar>(_T("")));

		str1 = _T("0");
		CHECK(XString<Tchar>(_T("")) != str1);
		CHECK(str1 != XString<Tchar>(_T("")));

		XString<Tchar> str2(_T("0"));
		CHECK(str2 == str1);
		CHECK(str1 == str2);

		str2 = _T("1");
		CHECK(str2 != str1);
		CHECK(str1 != str2);

		CHECK(XString<Tchar>(_T("ab")) == XString<Tchar>(_T("ab")));
		CHECK(XString<Tchar>(_T("safoiasdoi")) == XString<Tchar>(_T("safoiasdoi")));
	}

	TEST(Compare_Less) {
		size_t indexes[test_arr_size];
		size_t indexes_xstr[test_arr_size];

		for (size_t i = 0; i < test_arr_size; ++i)
			indexes[i] = indexes_xstr[i] = i;

		std::sort(indexes, indexes + test_arr_size, \
			[](const size_t l, const size_t r) {

			return Tstrcmp(test_strings[l], test_strings[r]) < 0;
		});

		XString<Tchar> rstr;
		std::sort(indexes_xstr, indexes_xstr + test_arr_size, \
			[&rstr](const size_t l, const size_t r) {
			XString<Tchar> lstr = test_strings[l];
			rstr = test_strings[r];

			return lstr < rstr;
		});

		CHECK(std::equal(indexes, indexes + test_arr_size, indexes_xstr));
	}

	TEST(Compare_More) {
		size_t indexes[test_arr_size];
		size_t indexes_xstr[test_arr_size];

		for (size_t i = 0; i < test_arr_size; ++i)
			indexes[i] = indexes_xstr[i] = i;

		std::sort(indexes, indexes + test_arr_size, \
			[](const size_t l, const size_t r) {

			return Tstrcmp(test_strings[l], test_strings[r]) > 0;
		});

		XString<Tchar> rstr;
		std::sort(indexes_xstr, indexes_xstr + test_arr_size, \
			[&rstr](const size_t l, const size_t r) {
			XString<Tchar> lstr = test_strings[l];
			rstr = test_strings[r];

			return lstr > rstr;
		});

		CHECK(std::equal(indexes, indexes + test_arr_size, indexes_xstr));
	}

	TEST(resize) {
		XString<Tchar> str;

		str.resize(0);
		CHECK(IsEmpty(str));
		CHECK(IsEqual(str, _T("")));

		str += _T('\0');
		CHECK(IsEmpty(str));
		CHECK(IsEqual(str, _T("")));

		str += _T("");
		CHECK(IsEmpty(str));
		CHECK(IsEqual(str, _T("")));

		str.resize(2);
		CHECK_EQUAL(2, str.size());

		str = _T("12");
		str.resize(20);
		CHECK_EQUAL(20, str.size());
		CHECK(!Tstrcmp(str.c_str(), _T("12")));

		str.resize(0);
		CHECK(str.empty());
		CHECK(!Tstrcmp(str.c_str(), _T("")));
	}
}