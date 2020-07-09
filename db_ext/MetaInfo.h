#pragma once
#include "MetaInfoBasic.h"

class CMetaInfo {
	CMetaInfoBasic meta_info;
	size_t invisible_fields_count;

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
	inline std::vector<size_t> getAllTableFieldsIndexes(const char *table_name) const;
	inline size_t getFieldSize(const size_t field) const noexcept;
	inline bool isPrimaryKey(const size_t field) const noexcept;

	inline const char *getPrimaryTableName() const;
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

	inline void bindPrimaryKeyValues(std::shared_ptr<const IDbResultSet> prim_key_values_src, \
							std::shared_ptr<IDbBindingTarget> binding_target) const;

	inline void bindPrimaryKeyValues(const size_t field, \
							std::shared_ptr<const IDbResultSet> prim_key_values_src, \
							std::shared_ptr<IDbBindingTarget> binding_target) const;

	inline void bindPrimaryKeyValuesWithOffset(const size_t field, \
							const size_t params_offset, \
							std::shared_ptr<const IDbResultSet> prim_key_values_src, \
							std::shared_ptr<IDbBindingTarget> binding_target) const;

	virtual ~CMetaInfo();
};

//*************************************************************

size_t CMetaInfo::getFieldsCount() const {

	assert(invisible_fields_count <= meta_info.getFieldsCount());
	return meta_info.getFieldsCount() - invisible_fields_count;
}

size_t CMetaInfo::getTablesCount() const {

	return meta_info.getTablesCount();
}

bool CMetaInfo::empty() const {

	return meta_info.empty();
}

std::shared_ptr<const IDbField> CMetaInfo::getField(const size_t field) const {

	assert(field < getFieldsCount());
	return meta_info.getField(field);
}

ImmutableString<char> CMetaInfo::getFieldName(const size_t field) const {

	assert(field < getFieldsCount());
	return meta_info.getFieldName(field);
}

ImmutableString<wchar_t> CMetaInfo::getFieldNameW(const size_t field) const {

	assert(field < getFieldsCount());
	return meta_info.getFieldNameW(field);
}

size_t CMetaInfo::getFieldIndexByName(const char *field_name) const {

	return meta_info.getFieldIndexByName(field_name);
}

size_t CMetaInfo::getFieldIndexByName(const char *field_name, const char *table_name) const {

	return meta_info.getFieldIndexByName(field_name, table_name);
}

ImmutableString<char> CMetaInfo::getTableName(const size_t field) const {

	assert(field < getFieldsCount());
	return meta_info.getTableName(field);
}

ImmutableString<wchar_t> CMetaInfo::getTableNameW(const size_t field) const {

	assert(field < getFieldsCount());
	return meta_info.getTableNameW(field);
}

std::vector<size_t> CMetaInfo::getAllTableFieldsIndexes(const char *table_name) const {

	return meta_info.getAllTableFieldsIndexes(table_name);
}

size_t CMetaInfo::getFieldSize(const size_t field) const noexcept {

	assert(field < getFieldsCount());
	return meta_info.getFieldSize(field);
}

bool CMetaInfo::isPrimaryKey(const size_t field) const noexcept {

	assert(field < getFieldsCount());
	return meta_info.isPrimaryKey(field);
}

const char *CMetaInfo::getPrimaryTableName() const {

	return meta_info.getPrimaryTableName();
}

void CMetaInfo::setPrimaryTable(const char *table_name) {

	meta_info.setPrimaryTable(table_name);
}

void CMetaInfo::setQueryConstantModifier(ImmutableString<char> modifier) {

	CMetaInfoBasic::setQueryConstantModifier(modifier);
}

void CMetaInfo::markFieldAsPrimaryKey(const size_t field) {

	assert(field < getFieldsCount());
	meta_info.markFieldAsPrimaryKey(field);
}

void CMetaInfo::clearAndAddFields(std::shared_ptr<const IDbResultSetMetadata> fields) {

	meta_info.clearAndAddFields(fields);
	invisible_fields_count = 0;
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

	assert(field < getFieldsCount());
	meta_info.getUpdateQueryForField(field, query);
}

void CMetaInfo::getDeleteQuery(std::string &query) const {

	meta_info.getDeleteQuery(query);
}

void CMetaInfo::bindPrimaryKeyValues(std::shared_ptr<const IDbResultSet> prim_key_values_src, \
								std::shared_ptr<IDbBindingTarget> binding_target) const {

	meta_info.bindPrimaryKeyValues(prim_key_values_src, binding_target);
}

void CMetaInfo::bindPrimaryKeyValues(const size_t field, \
								std::shared_ptr<const IDbResultSet> prim_key_values_src, \
								std::shared_ptr<IDbBindingTarget> binding_target) const {

	assert(field < getFieldsCount());
	meta_info.bindPrimaryKeyValues(field, prim_key_values_src, binding_target);
}

void CMetaInfo::bindPrimaryKeyValuesWithOffset(const size_t field, \
								const size_t params_offset, \
								std::shared_ptr<const IDbResultSet> prim_key_values_src, \
								std::shared_ptr<IDbBindingTarget> binding_target) const {

	meta_info.bindPrimaryKeyValuesWithOffset(field, params_offset, prim_key_values_src, \
												binding_target);
}