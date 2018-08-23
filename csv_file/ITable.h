#pragma once
#include <memory>
#include <string>

template <typename Tchar_> \
struct IRecord{
	virtual size_t size() const = 0;
	virtual const Tchar_ *getColValueAsCharArray(const size_t index) const = 0;
	virtual ~IRecord(){ }
};

template <typename Tstring_> \
struct IRecordWritable : public IRecord<typename Tstring_::value_type>{
	virtual Tstring_ &getColValueAsString(const size_t index) = 0;
	virtual ~IRecordWritable(){ }
};

template <typename Tchar_> \
struct IColumnsArray{
	virtual size_t getColumnsCount() const = 0;
	virtual size_t getColIndexByName(const Tchar_ *col_name) const = 0;
	virtual const Tchar_ *getColName(const size_t index) const = 0;
};

template <typename Tstring_> \
struct ITable : public IColumnsArray<typename Tstring_::value_type> {
	enum{
		NOT_FOUND = (size_t)-1
	};

	typedef typename Tstring_::value_type Tchar_;
	
	virtual size_t getRecordsCount() const = 0;
	virtual std::shared_ptr<const IRecord<Tchar_> > getRecord(const size_t index) const = 0;
	virtual std::shared_ptr<IRecord<Tchar_> > getRecord(const size_t index) = 0;
	virtual ~ITable(){ }
};
