#include <basic/tstream.h>
#include <csv_file/CsvTable.h>

int main() {
	const char locale_str[] = "uk_UA.UTF-8";
	
	setlocale(LC_ALL, locale_str);

	try {
		CCsvTable<std::wstring> t("1.csv", locale_str, L',', true);

		size_t rec_count = t.getRecordsCount();
		size_t cols_count = t.getColumnsCount();
		for (size_t i = 1; i < rec_count; ++i) {
			auto rec = t.getRecord(i);

			for (size_t j = 0; j < cols_count - 1; ++j)
				std::wcout << rec->getColValueAsCharArray(j) << L' ';
			std::wcout << L"<--"<< std::endl;
		}
	}
	catch (XException &e) {
		Tcerr << e.what() << std::endl;
		Tcin.get();
		return 1;
	}

	std::cin.get();
	return 0;
}