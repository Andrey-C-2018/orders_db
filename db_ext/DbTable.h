#pragma once
#include <vector>
#include <basic/Exception.h>
#include <table/ITable.h>
#include <db/IDbResultSet.h>
#include "Query.h"

class CDbTable : public ITable{
	CQuery query;
	std::shared_ptr<IDbResultSet> result_set;

	size_t curr_record;

	class CFieldsProperties : public IFieldsProperties {
		const CMetaInfo &meta_info;
		std::vector<int> sizes;
		int sizes_summ;
	public:
		CFieldsProperties(const CMetaInfo &meta_info_) : meta_info(meta_info_) {

			sizes.reserve(meta_info.getFieldsCount());
			sizes_summ = 0;
			for (size_t i = 0; i < meta_info.getFieldsCount(); ++i) {
				int sz = (int)meta_info.getFieldProperties(i).field_size;

				sizes.push_back(sz);
				sizes_summ += sz;
			}
		}

		int GetWidth(const size_t index) const override {

			assert(index < sizes.size());
			return sizes[index];
		}

		inline void SetWidth(const size_t index, const int width) {

			assert(index < sizes.size());
			sizes[index] = width;
		}

		ImmutableString<Tchar> GetName(const size_t index) const override {

			return meta_info.getFieldProperties(index).field_name;
		}

		size_t size() const override {

			return meta_info.getFieldsCount();
		}

		int GetSizesSumm() const override {

			return sizes_summ;
		}

		virtual ~CFieldsProperties(){ }
	};

	std::shared_ptr<CFieldsProperties> fields_props;

	enum Defaults {
		DEF_ON_CHANGE_HANDLERS_COUNT = 5
	};
	std::vector<ITableOnSizeChangedHandlerPtr> on_size_changed_handlers;
public:
	CDbTable(CQuery query_);

	inline size_t getCurrentRecordNo() const;

	const CQuery &getQuery() const;
	std::shared_ptr<const IFieldsProperties> GetFieldsProperties() const override;

	bool empty() const override;
	size_t GetFieldsCount() const override;
	size_t GetRecordsCount() const override;
	void ConnectOnSizeChangedHandler(ITableOnSizeChangedHandlerPtr handler) override;

	void AddField(const int field_size, const Tchar *field_name) override;
	void SetInsertString(ImmutableString<Tchar> insert_str);
	void AddRecord() override;
	void SetFieldSize(const size_t field, const int field_size) override;

	ImmutableString<Tchar> GetCellAsString(const size_t field, const size_t record) const override;
	void SetCell(const size_t field, const size_t record, const Tchar *value) override;

	void reload();
	virtual ~CDbTable();
};

//*****************************************************

size_t CDbTable::getCurrentRecordNo() const{

	return curr_record;
}

const CQuery &CDbTable::getQuery() const {

	return query;
}
