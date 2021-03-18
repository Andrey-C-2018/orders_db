#include <basic/TextConv.h>
#include <basic/Process.h>
#include <basic/IProperties.h>
#include <db_ext/DbTable.h>
#include <grid/Grid.h>
#include "Uploader.h"

CUploadException::CUploadException(const int err_code, \
									const Tchar *err_descr) : \
									XException(err_code, err_descr) { }

CUploadException::CUploadException(const CUploadException &obj) : \
									XException(obj) { }

CUploadException::~CUploadException() { }

//*****************************************************

Uploader::Uploader(const size_t hidden_count_) : \
					hidden_count(hidden_count_) { }

void Uploader::init(std::shared_ptr<CDbTable> db_table_, \
					IProperties *props) {

	assert(props);
	
	db_table = db_table_;
	Tstring buffer_w;
	
	const Tchar *value = props->getStringProperty(_T("output_file"), buffer_w);
	if (!value || (value && value[0] == _T('\0')))
		throw XException(0, _T("Параметр 'output_file' відсутній у config.ini"));

	path.resize(Tstrlen(value));

#pragma warning(push)
#pragma warning(disable: 4996)
	wcstombs(path.c_str_writable(), value, path.size());
#pragma warning(pop)

	const Tchar *p_excel_path = props->getStringProperty(_T("table_processor_path"), buffer_w);
	if (!p_excel_path || (p_excel_path && p_excel_path[0] == _T('\0')))
		throw XException(0, _T("Параметр 'table_processor_path' відсутній у config.ini"));

	excel_path.resize(Tstrlen(p_excel_path));

#pragma warning(push)
#pragma warning(disable: 4996)
	wcstombs(excel_path.c_str_writable(), p_excel_path, excel_path.size());
#pragma warning(pop)
}

void Uploader::open() {

	out.open(path.c_str(), std::ios::out | std::ios::trunc);
	if (!out.is_open()) {
		CUploadException e(CUploadException::E_FILE_OPEN, \
							_T("Неможливо відкрити файл: "));
		e << path.c_str();
		throw e;
	}
}

void Uploader::upload(const CGrid *grid) {

	assert(db_table);
	assert(grid);
	open();

	try {
		const size_t fields_count = db_table->GetFieldsCount() - hidden_count;
		const size_t records_count = db_table->GetRecordsCount();
		auto rs = db_table->getResultSet();
		const auto &meta_info = db_table->getQuery().getMetaInfo();

		addHeader();

		XString<char> buffer;
		if (fields_count) {
			out << "   <tr align=center>";
			for (size_t i = 0; i < fields_count; ++i) {
				out << " <td>";
				auto field_name = grid->GetFieldLabelAbs(i);
				UCS16_ToUTF8(field_name, -1, buffer);
				out << buffer.c_str() << "</td>";
			}
			out << " </tr>" << std::endl;
		}

		for (size_t i = 0; i < records_count; ++i) {
			out << "   <tr>";
			rs->gotoRecord(i);
			for (size_t j = 0; j < fields_count; ++j) {
				
				auto p_cell = meta_info.getField(j)->getValueAsImmutableString(rs);
				out << " <td>" << (!p_cell.isNull() ? p_cell.str : "") << "</td>";
			}
			out << " </tr>" << std::endl;
		}

		out << "  </table>" << std::endl;
		out << " </body>" << std::endl;
		out << "</html>" << std::endl;
	}
	catch (...) {
		out.close();
		throw;
	}

	if(out.is_open()) out.close();
}

void Uploader::execExcel() {

	std::string cmd_line = "1 ";
	cmd_line += path.c_str();
	exec(excel_path.c_str(), cmd_line.c_str(), "C:\\", false);
}

Uploader::~Uploader() { }
