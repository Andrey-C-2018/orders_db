#include "MatchingResultsTable.h"
#include "OutputWrapper.h"

CHTMLOutputWrapper::CHTMLOutputWrapper(const CMatchingResultsTable &table_) : table(table_){ }

std::ostream &CHTMLOutputWrapper::writeHeader(std::ostream &out){
	
out<< "<html>"<< std::endl;
out<< " <body>"<< std::endl;

const size_t cols_count = table.getColumnsCount();
if (cols_count) {
	out << "  <table border=1>" << std::endl;
	out << "   <tr>";
	for (size_t i = 0; i < cols_count; ++i)
		out<< " <td>"<< table.getColName(i)<< "</td>";
	out << "   </tr>"<< std::endl;
}

return out;
}

std::ostream &operator<<(std::ostream &out, const CHTMLOutputWrapper &obj){
const size_t rec_count = obj.table.getRecordsCount();

for(size_t i = 0; i < rec_count; ++i){
	const CMatchingResultsRecord rec = obj.table.getRecord(i);
	const size_t cols_count = rec.size();

	out<< "   <tr>";
	for(size_t j = 0; j < cols_count; ++j)
		out<< "<td>"<< rec.getColValueAsCharArray(j)<< "</td> ";
	out<< "   </tr>"<< std::endl;
}

return out;
}

std::ostream &CHTMLOutputWrapper::writeTail(std::ostream &out){
	
if(table.getColumnsCount())
	out<< "  </table>"<< std::endl;
out<< " </body>"<< std::endl;
out<< "</html>"<< std::endl;

return out;
}

CHTMLOutputWrapper::~CHTMLOutputWrapper(){ }

//**************************************************************

CCsvOutputWrapper::CCsvOutputWrapper(const CMatchingResultsTable &table_) : table(table_){ }

std::ostream &CCsvOutputWrapper::writeHeader(std::ostream &out) {
const size_t cols_count = table.getColumnsCount();

if (cols_count)
	writeCSValue(table.getColName(0), out);
for (size_t i = 1; i < cols_count; ++i) {
	out << ",";
	writeCSValue(table.getColName(i), out);
}
out<< std::endl;

return out;
}

std::ostream &operator<<(std::ostream &out, const CCsvOutputWrapper &obj){
const size_t rec_count = obj.table.getRecordsCount();

for(size_t i = 0; i < rec_count; ++i){
	const CMatchingResultsRecord rec = obj.table.getRecord(i);
	const size_t cols_count = rec.size();

	if(cols_count)
		obj.writeCSValue(rec.getColValueAsCharArray(0), out);
	for(size_t j = 1; j < cols_count; ++j){
		out << ",";
		obj.writeCSValue(rec.getColValueAsCharArray(j), out);
	}
	out<< std::endl;
}

return out;
}

std::ostream &CCsvOutputWrapper::writeTail(std::ostream &out) {

return out;
}

CCsvOutputWrapper::~CCsvOutputWrapper(){ }