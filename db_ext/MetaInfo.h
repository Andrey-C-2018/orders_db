#pragma once
#include "MetaInfoBasic.h"

class CMetaInfo {
	CMetaInfoBasic meta_info;
	std::vector<size_t> visible_fields;

public:
	CMetaInfo();

	CMetaInfo(const CMetaInfo &obj) = default;
	CMetaInfo(CMetaInfo &&obj) = default;
	CMetaInfo &operator=(const CMetaInfo &obj) = default;
	CMetaInfo &operator=(CMetaInfo &&obj) = default;

	inline size_t getFieldsCount() const;
	inline size_t getTablesCount() const;
	inline bool empty() const;

	inline std::shared_ptr<const IDbField> getField(const size_t field) const;
	inline ImmutableString<char> getFieldName(const size_t field) const;
	inline ImmutableString<wchar_t> getFieldNameW(const size_t field) const;
	inline size_t getFieldIndexByName(const char *field_name) const;
	inline size_t getFieldIndexByName(const char *field_name, const char *table_name) const;
	inline ImmutableString<char> getTableName(const size_t field) const;
	inline ImmutableString<wchar_t> getTableNameW(const size_t field) const;
	inline size_t getFieldSize(const size_t field) const noexcept;
	inline bool isPrimaryKey(const size_t field) const noexcept;

	inline void setPrimaryTable(const char *table_name);
	inline static void setQueryConstantModifier(ImmutableString<char> modifier);

	inline void markFieldAsPrimaryKey(const size_t field);
	inline void clearAndAddFields(std::shared_ptr<const IDbResultSetMetadata> fields);
	void addInvisibleFieldAsRef(const char *field_name, const char *table_name, \
						const char *ref_field_name, const char *ref_table_name, \
						const bool is_prim_key);

	inline size_t appendWherePartOfUpdateQuery(std::string &query, const char *table_alias) const;
	inline size_t appendWherePartOfUpdateQuery(const char *table_name, std::string &query, \
												const char *table_alias) const;

	inline void getUpdateQueryForField(const size_t field, std::string &query) const;
	inline void getDeleteQuery(std::string &query) const;

	inline void bindPrimaryKeyValues(const size_t field, \
							std::shared_ptr<const IDbResultSet> prim_key_values_src, \
							std::shared_ptr<IDbBindingTarget> binding_target) const;
	inline void bindPrimaryKeyValues(std::shared_ptr<const IDbResultSet> prim_key_values_src, \
							std::shared_ptr<IDbBindingTarget> binding_target) const;
	inline void bindPrimaryKeyValuesWithOffset(const size_t params_offset, \
							std::shared_ptr<const IDbResultSet> prim_key_values_src, \
							std::shared_ptr<IDbBindingTarget> binding_target) const;

	virtual ~CMetaInfo();
};

//*************************************************************

size_t CMetaInfo::getFieldsCount() const {

	assert(visible_fields.size() <= meta_info.getFieldsCount());
	return visible_fields.size();
}

size_t CMetaInfo::getTablesCount() const {

	return meta_info.getTablesCount();
}

bool CMetaInfo::empty() const {

	return visible_fields.empty();
}

std::shared_ptr<const IDbField> CMetaInfo::getField(const size_t field) const {

	return meta_info.getField(visible_fields[field]);
}

ImmutableString<char> CMetaInfo::getFieldName(const size_t field) const {

	return meta_info.getFieldName(visible_fields[field]);
}

ImmutableString<wchar_t> CMetaInfo::getFieldNameW(const size_t field) const {

	return meta_info.getFieldNameW(visible_fields[field]);
}

size_t CMetaInfo::getFieldIndexByName(const char *field_name) const {

	return meta_info.getFieldIndexByName(field_name);
}

size_t CMetaInfo::getFieldIndexByName(const char *field_name, const char *table_name) const {

	return meta_info.getFieldIndexByName(field_name, table_name);
}

ImmutableString<char> CMetaInfo::getTableName(const size_t field) const {

	return meta_info.getTableName(visible_fields[field]);
}

ImmutableString<wchar_t> CMetaInfo::getTableNameW(const size_t field) const {

	return meta_info.getTableNameW(visible_fields[field]);
}

size_t CMetaInfo::getFieldSize(const size_t field) const noexcept {

	return meta_info.getFieldSize(visible_fields[field]);
}

bool CMetaInfo::isPrimaryKey(const size_t field) const noexcept {

	return meta_info.isPrimaryKey(visible_fields[field]);
}

void CMetaInfo::setPrimaryTable(const char *table_name) {

	meta_info.setPrimaryTable(table_name);
}

void CMetaInfo::setQueryConstantModifier(ImmutableString<char> modifier) {

	CMetaInfoBasic::setQueryConstantModifier(modifier);
}

void CMetaInfo::markFieldAsPrimaryKey(const size_t field) {

	meta_info.markFieldAsPrimaryKey(visible_fields[field]);
}

void CMetaInfo::clearAndAddFields(std::shared_ptr<const IDbResultSetMetadata> fields) {

	meta_info.clearAndAddFields(fields);

	visible_fields.reserve(meta_info.getFieldsCount());
	for (size_t i = 0; i < meta_info.getFieldsCount(); ++i)
		visible_fields.push_back(i);
}

size_t CMetaInfo::appendWherePartOfUpdateQuery(std::string &query, \
												const char *table_alias) const {

	return meta_info.appendWherePartOfUpdateQuery(query, table_alias);
}

size_t CMetaInfo::appendWherePartOfUpdateQuery(const char *table_name, std::string &query, \
												const char *table_alias) const {

	return meta_info.appendWherePartOfUpdateQuery(table_name, query, table_alias);
}

void CMetaInfo::getUpdateQueryForField(const size_t field, std::string &query) const {

	meta_info.getUpdateQueryForField(visible_fields[field], query);
}

void CMetaInfo::getDeleteQuery(std::string &query) const {

	meta_info.getDeleteQuery(query);
}

void CMetaInfo::bindPrimaryKeyValues(const size_t field, \
								std::shared_ptr<const IDbResultSet> prim_key_values_src, \
								std::shared_ptr<IDbBindingTarget> binding_target) const {

	meta_info.bindPrimaryKeyValues(visible_fields[field], \
									prim_key_values_src, binding_target);
}
void CMetaInfo::bindPrimaryKeyValues(std::shared_ptr<const IDbResultSet> prim_key_values_src, \
								std::shared_ptr<IDbBindingTarget> binding_target) const {

	meta_info.bindPrimaryKeyValues(prim_key_values_src, binding_target);
}
void CMetaInfo::bindPrimaryKeyValuesWithOffset(const size_t params_offset, \
								std::shared_ptr<const IDbResultSet> prim_key_values_src, \
								std::shared_ptr<IDbBindingTarget> binding_target) const {

	meta_info.bindPrimaryKeyValues(params_offset, prim_key_values_src, binding_target);
}