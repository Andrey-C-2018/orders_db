#include "DbfTable.h"
#include <string>

COleDbException::COleDbException(HRESULT hr) : XException(0, L"") {
	LCID lcid;
	ULONG nRec = 0;
	CDBErrorInfo errInfo;
	BSTR errStr = nullptr;

	if (hr != S_OK) {
		errStr = ::SysAllocString(L"");
		lcid = GetUserDefaultLCID();
		errInfo.GetErrorRecords(&nRec);
		errInfo.GetAllErrorInfo(nRec - 1, lcid, &errStr);
	}

	if (errStr) {
		XException::operator<< (errStr);
		::SysFreeString(errStr);
	}
}

COleDbException::~COleDbException() { }

//****************************************************

COleDbConnection::COleDbConnection() {

	Init();
}

COleDbConnection::COleDbConnection(const wchar_t *database_folder) {

	Init();
	open(database_folder);
}

void COleDbConnection::open(const wchar_t *database_folder) {

	assert(database_folder);
	DWORD dwAttrib = GetFileAttributes(database_folder);

	if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
		XException e(0, "folder does not exist: ");
		e << database_folder;
		throw e;
	}

	std::wstring init_str = L"Provider=VFPOLEDB.1; Data Source=";
	init_str += database_folder;
	init_str += L"\\; Collating Sequence=MACHINE;";
	
	HRESULT hr = connection->conn.OpenFromInitializationString(init_str.c_str());
	if (hr != S_OK)
		throw COleDbException(hr);

	connection->is_opened = true;

	hr = session->session.Open(connection->conn);
	if (hr != S_OK)
		throw COleDbException(hr);

	session->is_opened = true;
}

void COleDbConnection::getTable(CDbfTable &dbf_table) {

	dbf_table.init(connection, session);
}

COleDbConnection::~COleDbConnection() { }

//****************************************************

CDbfTable::CDbfTable() : is_opened(false), fields_count(0), pIDataConvert(nullptr) {

	HRESULT hr = CoCreateInstance(CLSID_OLEDB_CONVERSIONLIBRARY, NULL, \
								CLSCTX_INPROC_SERVER, IID_IDataConvert, (void**)&pIDataConvert);
	if (hr != S_OK)
		throw COleDbException(hr);
}

CDbfTable::CDbfTable(std::shared_ptr<const Connection> connection_, \
						std::shared_ptr<const Session> session_) : \
							connection(connection_), session(session_), \
							is_opened(false), fields_count(0), pIDataConvert(nullptr) {

	assert(connection->is_opened);
	assert(session->is_opened);
	HRESULT hr = CoCreateInstance(CLSID_OLEDB_CONVERSIONLIBRARY, NULL, \
								CLSCTX_INPROC_SERVER, IID_IDataConvert, (void**)&pIDataConvert);
	if (hr != S_OK)
		throw COleDbException(hr);
}

void CDbfTable::init(std::shared_ptr<const Connection> connection_, \
						std::shared_ptr<const Session> session_) {

	assert(connection_->is_opened);
	assert(session_->is_opened);

	close();

	this->connection = connection_;
	this->session = session_;
}

void CDbfTable::open(const wchar_t *table_name) {

	assert(table_name && table_name[0] != L'\0');

	close();
	
	HRESULT hr = table.Open(session->session, table_name);
	if (hr != S_OK)
		throw COleDbException(hr);

	is_opened = true;
	fields_count = table.GetColumnCount();
}

const char *CDbfTable::getCStringCellValue(const size_t field) {

	DBTYPE type;
	table.GetColumnType(field + 1, &type);
	assert(type == DBTYPE_STR);

	return (CHAR*)table.GetValue(field + 1);
}

const wchar_t *CDbfTable::getCWStringCellValue(const size_t field) {

	DBTYPE type;
	table.GetColumnType(field + 1, &type);
	assert(type == DBTYPE_WSTR);

	return (WCHAR*)table.GetValue(field + 1);
}

int CDbfTable::getIntCellValue(const size_t field, bool &is_null) {

	assert(field < fields_count);

	DBTYPE type;
	table.GetColumnType(field + 1, &type);
	assert(type == DBTYPE_I2 || type == DBTYPE_I4 || type == DBTYPE_NUMERIC);

	void *src_value = table.GetValue(field + 1);

	int buffer;
	DBLENGTH buffer_size = 0;
	DBSTATUS dbsStatus;
	
	pIDataConvert->DataConvert(type, DBTYPE_I4,	sizeof(LARGE_INTEGER), \
								&buffer_size, src_value, &buffer, sizeof(int), \
								DBSTATUS_S_OK, &dbsStatus, 0, 0, \
								DBDATACONVERT_DEFAULT);

	is_null = dbsStatus == DBSTATUS_S_ISNULL;
	return buffer;
}

const char *CDbfTable::getNumericCellValue(const size_t field, char *buffer, \
											const size_t overall_size) {

	assert(field < fields_count);

	DBTYPE type;
	table.GetColumnType(field + 1, &type);
	assert(type == DBTYPE_NUMERIC);

	void *src_value = table.GetValue(field + 1);

	DBLENGTH buffer_size = 0;
	DBSTATUS dbsStatus;

	pIDataConvert->DataConvert(type, DBTYPE_STR, overall_size, \
								&buffer_size, src_value, buffer, overall_size + 1, \
								DBSTATUS_S_OK, &dbsStatus, \
								0,   // bPrecision (used for DBTYPE_NUMERIC only)  
								0,   // bScale (used for DBTYPE_NUMERIC only)  
								DBDATACONVERT_DECIMALSCALE);

	return dbsStatus == DBSTATUS_S_ISNULL ? nullptr : buffer;
}

const char *CDbfTable::getDateCellValue(const size_t field, char *buffer) {

	DBTYPE type;
	table.GetColumnType(field + 1, &type);
	assert(type == DBTYPE_DBDATE);

	void *src_value = table.GetValue(field + 1);

	DBLENGTH buffer_size = 0;
	DBSTATUS dbsStatus;

	pIDataConvert->DataConvert(type, DBTYPE_STR, DATE_SIZE, \
								&buffer_size, src_value, buffer, DATE_SIZE, \
								DBSTATUS_S_OK, &dbsStatus, 0, 0, \
								DBDATACONVERT_DEFAULT);
	
	return dbsStatus == DBSTATUS_S_ISNULL ? nullptr : buffer;
}

void CDbfTable::close() {

	if (is_opened) {
		table.Close();
		is_opened = false;
		fields_count = 0;
	}
}

CDbfTable::~CDbfTable(){

	close();
	pIDataConvert->Release();
}
