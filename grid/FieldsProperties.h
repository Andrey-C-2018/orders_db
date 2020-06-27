#pragma once
#include <vector>
#include <algorithm>
#include <assert.h>
#include <table/ITable.h>
#include "ISizes.h"

class CFieldsProperties : public IFieldsProperties {
	enum {
		DEF_FIELDS_COUNT = 10
	};

	struct FieldProps {
		int field_size;
		std::vector<Tchar> field_name;
		bool visible;
	};

	std::shared_ptr<const ITable> data_table;
	std::vector<FieldProps> fields_props;
	std::vector<size_t> indexes;

	int summ;

	int addition;
	int multiplier;
public:
	CFieldsProperties() : summ(0), addition(0), multiplier(1) { }

	CFieldsProperties(std::shared_ptr<const ITable> data_table_) : \
							summ(0), addition(0), multiplier(1), \
							data_table(data_table_) {

		SyncWithDataTable();
	}

	void SyncWithDataTable() {
		size_t fields_count = data_table->GetFieldsCount();

		fields_props.clear();
		indexes.clear();
		summ = 0;

		if (!fields_count) {
			fields_props.reserve(DEF_FIELDS_COUNT);
			indexes.reserve(DEF_FIELDS_COUNT);
		}
		else {
			fields_props.reserve(fields_count);
			indexes.reserve(fields_count);

			std::vector<Tchar> empty_field_name;
			for (size_t i = 0; i < fields_count; ++i) {
				int field_size = (int)data_table->GetFieldMaxLengthInChars(i);

				field_size *= multiplier;
				field_size += addition;

				fields_props.push_back(FieldProps{ field_size, empty_field_name, true });
				indexes.push_back(i);
				summ += field_size;
			}
		}
	}

	inline void OnFieldAddedToDataTable(const size_t index) {

		assert(index <= fields_props.size());

		int field_size = (int)data_table->GetFieldMaxLengthInChars(index);
		std::vector<Tchar> empty_field_name;

		field_size *= multiplier;
		field_size += addition;

		fields_props.insert(fields_props.begin() + index, \
							FieldProps{ field_size, empty_field_name, true });
		if (index == fields_props.size())
			indexes.push_back(index);
		else {
			size_t fields_props_size = fields_props.size();
			std::for_each(indexes.begin(), indexes.end(), \
						[index, fields_props_size](size_t &index_item) {
							index_item += index == fields_props_size;
						});

			indexes.insert(indexes.begin() + index, index);
		}
		summ += field_size;
	}

	inline size_t GetFieldIndex(const size_t index) const {

		return indexes[index];
	}

	inline size_t GetRelativeFieldIndex(const size_t abs_index) const {

		auto p = std::find(indexes.cbegin(), indexes.cend(), abs_index);
		return p != indexes.cend() ? p - indexes.cbegin() : -1;
	}

	int GetWidth(const size_t index) const override {

		assert(index < indexes.size());
		return fields_props[GetFieldIndex(index)].field_size;
	}

	inline void SetWidth(const size_t index, const int field_width) {

		assert(index < indexes.size());
		summ -= fields_props[GetFieldIndex(index)].field_size;
		summ += field_width;

		fields_props[GetFieldIndex(index)].field_size = field_width;
	}

	inline bool IsFieldHidden(const size_t index) const {

		return !fields_props[index].visible;
	}

	inline void HideField(const size_t index) {

		assert(index < fields_props.size());
		assert(fields_props.size());

		if (fields_props[index].visible) {
			indexes.erase(std::remove(indexes.begin(), indexes.end(), index));
			fields_props[index].visible = false;

			summ -= fields_props[index].field_size;
		}
	}

	void TransformAllSizes(const int addition, const int multiplier) override {

		for (auto p = fields_props.begin(); p != fields_props.end(); ++p) {

			p->field_size -= this->addition;
			p->field_size /= this->multiplier;

			p->field_size *= multiplier;
			p->field_size += addition;
		};

		summ -= this->addition * (int)fields_props.size();
		summ /= this->multiplier;
		summ *= multiplier;
		summ += addition * (int)fields_props.size();

		this->addition = addition;
		this->multiplier = multiplier;
	}

	inline ImmutableString<Tchar> GetFieldNameAbs(const size_t index) const {

		assert(index < fields_props.size());
		assert(fields_props.size());
		assert(data_table);

		if (fields_props[index].field_name.size()) {
			ImmutableString<Tchar> field_name(&fields_props[index].field_name[0], \
											fields_props[index].field_name.size());
			return field_name;
		}

		return data_table->GetFieldNameAsImmutableStr(index);
	}

	ImmutableString<Tchar> GetFieldName(const size_t index) const override {

		assert(index < indexes.size());
		size_t exact_index = GetFieldIndex(index);
		return GetFieldNameAbs(exact_index);
	}

	inline void SetFieldName(const size_t index, const Tchar *new_name) {

		assert(index < indexes.size());
		size_t size = Tstrlen(new_name);
		auto &name = fields_props[GetFieldIndex(index)].field_name;

		name.resize(size + 1);
		Tstrncpy(&name[0], new_name, size);
	}

	size_t size() const override { return GetFieldsCount(); }

	inline size_t GetFieldsCount() const {

		return indexes.size();
	}

	inline size_t GetAbsoluteFieldsCount() const {

		return fields_props.size();
	}

	int GetFieldsSizesSumm() const override {

		return summ;
	}

	virtual ~CFieldsProperties() { }
};
