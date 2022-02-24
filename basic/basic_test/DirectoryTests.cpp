#include <vector>
#include <algorithm>
#include <iostream>
#include <UnitTest++/UnitTest++.h>
#include <basic/Directory.h>
#include <basic/Exception.h>
#include <basic/tstream.h>

const Tchar *all_files[] = {_T("nested"), _T("1"), _T("1.txt"), _T("2.txt"), \
							_T("2.tx"), _T("abc.dbf"), _T("abc")};
const size_t all_files_count = sizeof(all_files) / sizeof(const Tchar *);

inline std::vector<Tstring> getDirContents(const Tchar *path) {

	XDirectory dir;
	std::vector<Tstring> output;

	bool files_exist = dir.getFirstFile(path);
	while (files_exist) {
		output.emplace_back(dir.getFileName());
		files_exist = dir.getNextFile();
	}
	std::sort(output.begin(), output.end());
	return output;
}

void testEqualContents(std::vector<Tstring> &etalon, const Tchar *path) {

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

std::vector<Tstring> copyIfEndsWith(const Tchar *ext) {

	size_t ext_len = Tstrlen(ext);

	std::vector<Tstring> etalon;
	std::copy_if(all_files, all_files + all_files_count, \
					std::back_inserter(etalon), \
			[ext, ext_len](const Tchar *file) -> bool {

		return endsWith(file, ext, ext_len);
	});

	return etalon;
}

SUITE(DirectoryTests) {

	TEST(openDirNoSlashes) {
		std::vector<Tstring> etalon(all_files, all_files + all_files_count);
		testEqualContents(etalon, _T("test_data"));
	}

	TEST(openDirEndingSlash) {
		std::vector<Tstring> etalon(all_files, all_files + all_files_count);
		testEqualContents(etalon, _T("test_data/"));
	}

	TEST(openDirNested) {
		try {
			auto output = getDirContents(_T("test_data/nested"));
			CHECK_EQUAL(1, output.size());
		}
		catch(XException &e) {

			std::cerr << e.what() << std::endl;
					CHECK(1 == 0);
		}
	}

	TEST(openDirAllFilesTemplate) {
		std::vector<Tstring> etalon(all_files, all_files + all_files_count);
		testEqualContents(etalon, _T("test_data/*"));
	}

	TEST(openDirFilesTemplateTXT) {

		auto etalon = copyIfEndsWith(_T("txt"));
		testEqualContents(etalon, _T("test_data/*.txt"));
	}

	TEST(openDirFilesTemplateTX_DBF) {

		auto etalon = copyIfEndsWith(_T("tx"));
		testEqualContents(etalon, _T("test_data/*.tx"));

		etalon = copyIfEndsWith(_T("dbf"));
		testEqualContents(etalon, _T("test_data/*.dbf"));
	}

	TEST(openDirNotExistingTemplate) {

		std::vector<Tstring> output;
		try {
			output = getDirContents(_T("test_data/*.xyz"));
			CHECK(output.empty());
		}
		catch(XException &e) {

			Tcerr << e.what() << std::endl;
			CHECK(1 == 0);
		}

		bool nominal = false;
		try { getDirContents(_T("test_data/*-=")); }
		catch(XException &e) { nominal = true; }
		CHECK(nominal);

		nominal = false;
		try { getDirContents(_T("test_data/*.*")); }
		catch(XException &e) { nominal = true; }
		CHECK(nominal);

		nominal = false;
		try { getDirContents(_T("test_data/xyz*")); }
		catch(XException &e) { nominal = true; }
		CHECK(nominal);

		nominal = false;
		try { getDirContents(_T("test_data/ab*.dbf")); }
		catch(XException &e) { nominal = true; }
		CHECK(nominal);
	}
}
