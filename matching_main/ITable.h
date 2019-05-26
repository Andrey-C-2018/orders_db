#pragma once
#include <memory>
#include <string>

struct IRecord{
	virtual size_t size() const = 0;
	virtual const char *getColValueAsCharArray(const size_t index) const = 0;
	virtual ~IRecord(){ }
};

struct IRecordWritable : public IRecord{
	virtual std::string &getColValueAsString(const size_t index) = 0;
	virtual ~IRecordWritable(){ }
};

struct IColumnsArray{
	virtual size_t getColumnsCount() const = 0;
	virtual size_t getColIndexByName(const char *col_name) const = 0;
	virtual const char *getColName(const size_t index) const = 0;
};

struct ITable : public IColumnsArray{
	enum{
		NOT_FOUND = (size_t)-1
	};
	
	virtual size_t getRecordsCount() const = 0;
	virtual std::shared_ptr<const IRecord> getRecord(const size_t index) const = 0;
	virtual std::shared_ptr<IRecord> getRecord(const size_t index) = 0;
	virtual ~ITable(){ }
};
