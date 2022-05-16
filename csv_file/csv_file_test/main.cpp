#include <basic/tstream.h>
#include <basic/locale_init.h>
#include <csv_file/CsvTable.h>

int main() {
	setLocaleToCurrOS_Locale();

	try {
		CCsvTable<Tstring> t("1.csv", getOS_Locale(), _T(','), true, true);

		size_t rec_count = t.getRecordsCount();
		size_t cols_count = t.getColumnsCount();
		for (size_t i = 1; i < rec_count; ++i) {
			auto rec = t.getRecord(i);

			for (size_t j = 0; j < cols_count - 1; ++j)
				Tcout << rec->getColValueAsCharArray(j) << _T(' ');
			Tcout << _T("<--") << std::endl;
		}
	}
	catch (XException &e) {
		Tcerr << e.what() << std::endl;
		Tcin.get();
		return 1;
	}

	Tcin.get();
	return 0;
}