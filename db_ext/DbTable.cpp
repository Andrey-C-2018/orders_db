#include "DbTable.h"

class CFieldsProperties : public IFieldsProperties {
	const CMetaInfo &meta_info;
	std::vector<int> sizes;
	int sizes_summ;
public:
	CFieldsProperties(const CMetaInfo &meta_info_) : meta_info(meta_info_){
       
		sizes.reserve(meta_info.getFieldsCount());
		sizes_summ = 0;
		for(size_t i = 0; i < meta_info.getFieldsCount(); ++i){
			int sz = (int)meta_info.getField(i).field_size;

			sizes.push_back(sz);
			sizes_summ += sz;
		}
	}

	int GetWidth(const size_t index) const override{

		assert(index < sizes.size());
		return sizes[index];
	}

	inline void SetWidth(const size_t index, const int width){

		assert(index < sizes.size());
		sizes[index] = width;
	}

	ImmutableString<Tchar> GetName(const size_t index) const override{

		return meta_info.getField().field_name;
	}

	size_t size() const override{

		return meta_info.getFieldsCount();
	}

	int GetSizesSumm() const override{

		return summ;
	}

	virtual ~CFieldsProperties();
};

//****************************************************

CDbTable::CDbTable(CQuery query_) : curr_record(0), query(std::move(query_)){

	fields_props = std::make_shared<CFieldsProperties>(query.getMetaInfo());
	result_set = query.exec();

	on_size_changed_handlers.reserve(DEF_ON_CHANGE_HANDLERS_COUNT);
}

std::shared_ptr<const IFieldsProperties> CDbTable::GetFieldsProperties() const{

	return fields_props;
}

bool CDbTable::empty() const override{

	bool no_fields = query.getMetaInfo().empty();
	return no_fields && !result_set->getRecordsCount();
}

size_t CDbTable::GetFieldsCount() const{

	return query.getMetaInfo().getFieldsCount();
}

size_t CDbTable::GetRecordsCount() const{

	return result_set->getRecordsCount();
}

void CDbTable::ConnectOnSizeChangedHandler(ITableOnSizeChangedHandlerPtr handler){

	on_size_changed_handlers.push_back(handler);
}

void CDbTable::AddField(const int field_size, const Tchar *field_name){

	throw XException(0, _T("Not supported"));
}

void CDbTable::AddRecord(){
	size_t records_count = result_set->getRecordsCount();

	for (auto &handler : on_size_changed_handlers)
		handler->OnSizeChanged(query.getMetaInfo().getFieldsCount(), \
					records_count);
}

ImmutableString<Tchar> CDbTable::GetCellAsString(const size_t field, const size_t record) const{

	result_set->gotoRecord(record);
	auto field_ptr = query.getMetaInfo().getField(field);
	return field_ptr->getValueAsImmutableString(result_set);
}

void CDbTable::SetCell(const size_t field, const size_t record, const Tchar *value){

	result_set->gotoRecord(record);
	auto update_stmt = query.getUpdateStmt(field, result_set);

	auto updated_field_ptr = query.getMetaInfo().getField(field);
	updated_field_ptr->bindValueAsString(update_stmt, 0, value);
	update_stmt->execScalar();

	reload();
}

void CDbTable::reload(){

	result_set = query.exec();
}

CDbTable::~CDbTable(){ }
