#pragma once
#include <atldbcli.h>
#include <msdadc.h>   // for IDataConvert  
#include <msdaguid.h>
#include <memory>
#include <assert.h>
#include <basic/Exception.h>

class COleDbException : public XException {
public:

	COleDbException(HRESULT hr);

	COleDbException(const COleDbException &obj) = default;
	COleDbException(COleDbException &&obj) = default;
	COleDbException &operator=(const COleDbException &obj) = default;
	COleDbException &operator=(COleDbException &&obj) = default;

	~COleDbException();
};

class CCOMHelper final {
public:
	CCOMHelper() { ::CoInitialize(nullptr); }
	~CCOMHelper() { ::CoUninitialize(); }
};

struct Connection {
	CDataSource conn;
	bool is_opened;
	inline Connection() noexcept : is_opened(false) { }
};

struct Session {
	CSession session;
	bool is_opened;
	inline Session() noexcept : is_opened(false) { }
};

class CDbfTable {
	std::shared_ptr<const Connection> connection;
	std::shared_ptr<const Session> session;

	CTable<CDynamicAccessor> table;
	IDataConvert* pIDataConvert;
	bool is_opened;
	size_t fields_count;

public:
	enum ColumnsTypesSizes {
		INT_SIZE = 4, 
		DATE_SIZE = 10
	};

	CDbfTable();
	CDbfTable(std::shared_ptr<const Connection> connection_, \
				std::shared_ptr<const Session> session_);
	void init(std::shared_ptr<const Connection> connection_, \
				std::shared_ptr<const Session> session_);
	void open(const wchar_t *table_name);
	
	CDbfTable(const CDbfTable &obj) = delete;
	CDbfTable(CDbfTable &&obj) = delete;
	CDbfTable &operator=(const CDbfTable &obj) = delete;
	CDbfTable &operator=(CDbfTable &&obj) = delete;

	inline size_t getFieldsCount() const;
	inline const wchar_t *getFieldName(const size_t field) const;

	inline bool gotoFirstRecord();
	inline bool gotoNextRecord();

	const char *getCStringCellValue(const size_t field);
	const wchar_t *getCWStringCellValue(const size_t field);
	int getIntCellValue(const size_t field, bool &is_null);
	const char *getNumericCellValue(const size_t field, char *buffer, \
									const size_t overall_size);
	const char *getDateCellValue(const size_t field, char *buffer);

	void close();
	virtual ~CDbfTable();
};

class COleDbConnection {
	std::shared_ptr<Connection> connection;
	std::shared_ptr<Session> session;

	inline void Init();
public:

	COleDbConnection();
	COleDbConnection(const wchar_t *database_folder);

	COleDbConnection(const COleDbConnection &obj) = default;
	COleDbConnection(COleDbConnection &&obj) = default;
	COleDbConnection &operator=(const COleDbConnection &obj) = default;
	COleDbConnection &operator=(COleDbConnection &&obj) = default;

	void open(const wchar_t *database_folder);
	void getTable(CDbfTable &dbf_table);

	virtual ~COleDbConnection();
};

//****************************************************

void COleDbConnection::Init() {

	connection = std::shared_ptr<Connection>(new Connection, \
		[](Connection *ptr) {

		if (ptr->is_opened) ptr->conn.Close();
		delete ptr;
	});

	session = std::shared_ptr<Session>(new Session, \
		[](Session *ptr) {

		if (ptr->is_opened) ptr->session.Close();
		delete ptr;
	});
}

//****************************************************

size_t CDbfTable::getFieldsCount() const {

	return fields_count;
}

const wchar_t *CDbfTable::getFieldName(const size_t field) const {

	assert(field < fields_count);
	return table.GetColumnName(field + 1);
}

bool CDbfTable::gotoFirstRecord() {

	HRESULT hr = table.MoveFirst();
	return SUCCEEDED(hr) && hr != DB_S_ENDOFROWSET;
}

bool CDbfTable::gotoNextRecord() {

	HRESULT hr = table.MoveNext();
	return SUCCEEDED(hr) && hr != DB_S_ENDOFROWSET;
}