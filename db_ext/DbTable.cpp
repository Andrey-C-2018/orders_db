#include "DbTable.h"

class CBindValuePredicate final{
	std::shared_ptr<const CDbResultSet> result_set;
	size_t curr_record;
public:
	inline void operator()(const size_t field, const size_t binding_param_no, \
				std::shared_ptr<IDbBindingTarget> binding_target) const{

		result_set->getValueAndBindItTo(field, curr_record, \
						binding_param_no, binding_target );
	}
};

//****************************************************

class CFieldsProperties : public IFieldsProperties {
	const CMetaInfo &meta_info;
	std::vector<int> sizes;
	int sizes_summ;
public:
	CFieldsProperties(const CMetaInfo &meta_info_) : meta_info(meta_info_){
       
		sizes.reserve(meta_info.getFieldsCount());
		sizes_summ = 0;
		for(size_t i = 0; i < meta_info.getFieldsCount(); ++i){
			int sz = (int)meta_info.getField().field_size;

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
}

std::shared_ptr<const IFieldsProperties> CDbTable::GetFieldsProperties() const{

	return fields_props;
}

bool CDbTable::empty() const override{

	return query.getMetaInfo().empty();
}

size_t CDbTable::GetFieldsCount() const{

	return query.getMetaInfo().getFieldsCount();
}

size_t CDbTable::GetRecordsCount() const{

	return query.getMetaInfo().getRecordsCount();
}

void CDbTable::ConnectOnSizeChangedHandler(ITableOnSizeChangedHandlerPtr handler){

}

void CDbTable::AddField(const int field_size, const Tchar *field_name){

	throw XException(0, _T("Not supported"));
}

void CDbTable::AddRecord(){ }

ImmutableString<Tchar> CDbTable::GetCellAsString(const size_t field, const size_t record) const{

}

void CDbTable::SetCell(const size_t field, const size_t record, const Tchar *value){

}

CDbTable::~CDbTable(){ }
