#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <UnitTest++/UnitTest++.h>
#include <basic/Directory.h>
#include <basic/Exception.h>

const wchar_t *all_files[] = {L"nested", L"1", L"1.txt", L"2.txt", L"2.tx", L"abc.dbf", L"abc"};
const size_t all_files_count = sizeof(all_files) / sizeof(const char *);

inline std::vector<std::wstring> getDirContents(const wchar_t *path) {

	XDirectory dir;
	std::vector<std::wstring> output;

	bool files_exist = dir.getFirstFile(path);
	while (files_exist) {
		output.emplace_back(dir.getFileName());
		files_exist = dir.getNextFile();
	}
	std::sort(output.begin(), output.end());
	return output;
}

void testEqualContents(std::vector<std::wstring> &etalon, const wchar_t *path) {

	std::sort(etalon.begin(), etalon.end());
	try {
		auto output = getDirContents(path);

		CHECK_EQUAL(etalon.size(), output.size());
		if (all_files_count == output.size()) {
			bool is_equal = std::equal(etalon.cbegin(), etalon.cend(), output.cbegin());
			CHECK (is_equal);
		}
	}
	catch(XException &e) {

		std::cerr << e.what() << std::endl;
		CHECK(1 == 0);
	}
}

std::vector<std::wstring> copyIfEndsWith(const wchar_t *ext) {

	size_t ext_len = wcslen(ext);

	std::vector<std::wstring> etalon;
	std::copy_if(all_files, all_files + all_files_count, \
					std::back_inserter(etalon), \
			[ext, ext_len](const wchar_t *file) -> bool {

		return endsWith(file, ext, ext_len);
	});

	return etalon;
}

SUITE(DirectoryTests) {

	TEST(openDirNoSlashes) {
		std::vector<std::wstring> etalon(all_files, all_files + all_files_count);
		testEqualContents(etalon, L"test_data");
	}

	TEST(openDirEndingSlash) {
		std::vector<std::wstring> etalon(all_files, all_files + all_files_count);
		testEqualContents(etalon, L"test_data/");
	}

	TEST(openDirNested) {
		try {
			auto output = getDirContents(L"test_data/nested");
			CHECK_EQUAL(1, output.size());
		}
		catch(XException &e) {

			std::cerr << e.what() << std::endl;
					CHECK(1 == 0);
		}
	}

	TEST(openDirAllFilesTemplate) {
		std::vector<std::wstring> etalon(all_files, all_files + all_files_count);
		testEqualContents(etalon, L"test_data/*");
	}

	TEST(openDirFilesTemplateTXT) {

		auto etalon = copyIfEndsWith(L"txt");
		testEqualContents(etalon, L"test_data/*.txt");
	}

	TEST(openDirFilesTemplateTX_DBF) {

		auto etalon = copyIfEndsWith(L"tx");
		testEqualContents(etalon, L"test_data/*.tx");

		etalon = copyIfEndsWith(L"dbf");
		testEqualContents(etalon, L"test_data/*.dbf");
	}

	TEST(openDirNotExistingTemplate) {

		std::vector<std::wstring> output;
		try {
			output = getDirContents(L"test_data/*.xyz");
			CHECK(output.empty());
		}
		catch(XException &e) {

			std::cerr << e.what() << std::endl;
			CHECK(1 == 0);
		}

		bool nominal = false;
		try { getDirContents(L"test_data/*-="); }
		catch(XException &e) { nominal = true; }
		CHECK(nominal);

		nominal = false;
		try { getDirContents(L"test_data/*.*"); }
		catch(XException &e) { nominal = true; }
		CHECK(nominal);

		nominal = false;
		try { getDirContents(L"test_data/xyz*"); }
		catch(XException &e) { nominal = true; }
		CHECK(nominal);

		nominal = false;
		try { getDirContents(L"test_data/ab*.dbf"); }
		catch(XException &e) { nominal = true; }
		CHECK(nominal);
	}
}
