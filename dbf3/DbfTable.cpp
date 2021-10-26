#include <string>
#include <cstring>
#include "DbfTable.h"

DbfTableException::DbfTableException(const int err_code, const char *err_descr) : \
						XException(err_code, err_descr) { }

DbfTableException::DbfTableException(const DbfTableException &obj) : \
						XException(obj) { }

DbfTableException::~DbfTableException() { }

//***************************************************************

DbfTable::DbfTable() : rec_remove_mark(false), curr_rec((size_t)-1) { }

DbfTable::DbfTable(const char *table_path) : rec_remove_mark(false), curr_rec((size_t)-1) {

	open(table_path);
}

void DbfTable::open(const char *table_path) {

	close();

	if (!table_path || (table_path && table_path[0] == '\0'))
		throw DbfTableException(DbfTableException::E_PATH_IS_NULL, \
							"the path to dbf file is NULL or empty");

	f.open(table_path, std::ios::binary);
	if (!f.is_open()) {
		DbfTableException e(DbfTableException::E_NO_DBF_FILE, \
							"the specified dbf-file cannot be opened: ");
		e << table_path;
		throw e;
	}

	readHeader(table_path);
	initFields();
	gotoFirstRecord();
}

void DbfTable::readHeader(const char *table_path) {

	assert(f.is_open());
	if(!header) header = std::make_unique<DBaseHeader>();

	f.read((char *)header.get(), sizeof(DBaseHeader));
	if (!(header->version == 3 || header->version == 0x83)) {
		DbfTableException e(DbfTableException::E_VERSION, \
			"the version of this dbf-file is not supported. Path: ");

		e << table_path;
		e << " . Version: ";
		if (header->version == 0x8b)
			e << "dBase IV";
		else
			e << header->version;
		throw e;
	}
}

void DbfTable::initFields() {

	assert(f.is_open());
	assert(header->length > 0);
	DBaseField field;
	size_t fields_count = ((size_t)header->length - 1) / 32 - 1;

	fields.reserve(fields_count);
	field_buffers.reserve(fields_count);
	for (size_t i = 0; i < fields_count; ++i) {

		f.read((char *)&field.name[0], sizeof(field.name));
		f.read((char *)&field.type, sizeof(field.type));
		f.read((char *)&field.gap[0], sizeof(field.gap));
		f.read((char *)&field.length, sizeof(field.length));
		f.read((char *)&field.dec_point, sizeof(field.dec_point));
		f.read((char *)&field.fill[0], sizeof(field.fill));
		fields.push_back(field);

		std::vector<char> field_buffer;
		field_buffer.resize((size_t)field.length + 1);
		field_buffers.emplace_back(field_buffer);
	}
}

bool DbfTable::fetchNext() {

	assert(f.is_open());
	if(f.eof()) return false;

	char rec_remove_ch = 0;
	f.get(rec_remove_ch);
	if(rec_remove_ch == 0x1a || f.eof()) return false;

	rec_remove_mark = rec_remove_ch == 0x2a;

	for(size_t i = 0; i < field_buffers.size(); ++i) {
		f.read(&field_buffers[i][0], fields[i].length);
		field_buffers[i][fields[i].length] = '\0';
		
		if (fields[i].type == DBF_FIELD_CHAR || fields[i].type == DBF_FIELD_MEMO)
			rtrim(&field_buffers[i][0], fields[i].length);
		if (fields[i].type == DBF_FIELD_NUM)
			ltrim(&field_buffers[i][0], fields[i].length);

		if(fields[i].length != f.gcount()) {
			DbfTableException e(DbfTableException::E_WRONG_FORMAT, "the current record size is wrong: ");
			e << (size_t)f.gcount() << " bytes instead of " << fields[i].length;
			throw e;
		}
	}
	++curr_rec;
	return true;	
}

void DbfTable::close() {

	if(f.is_open()) f.close();
	
	if (header) {
		header->version = 0;
		header->last_update[0] = 0;
		header->last_update[1] = 0;
		header->last_update[2] = 0;
		header->length = 0;
		header->recs_count = 0;
		header->rec_len = 0;
	}
	
	fields.clear();
	field_buffers.clear();
}

DbfTable::~DbfTable(){

	close();
}
