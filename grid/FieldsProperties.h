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
	int multiplier;
public:
	CFieldsProperties() : summ(0), multiplier(1) { }

	CFieldsProperties(std::shared_ptr<const ITable> data_table_) : summ(0), multiplier(1), \
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

		if (fields_props[index].visible) {
			indexes.erase(std::remove(indexes.begin(), indexes.end(), index));
			fields_props[index].visible = false;

			summ -= fields_props[index].field_size;
		}
	}

	void MultiplyAllSizesBy(const int multiplier) override {

		std::for_each(fields_props.begin(), fields_props.end(), \
			[multiplier](FieldProps &field_item) {

			field_item.field_size *= multiplier;
		});

		summ *= multiplier;
	}

	void SetNewFieldMultiplier(const int multiplier) override {

		this->multiplier = multiplier;
	}

	ImmutableString<Tchar> GetFieldName(const size_t index) const override {

		assert(index < indexes.size());
		assert(data_table);
		size_t exact_index = GetFieldIndex(index);

		if (fields_props[exact_index].field_name.size()) {
			ImmutableString<Tchar> field_name(&fields_props[exact_index].field_name[0], \
												fields_props[exact_index].field_name.size());
			return field_name;
		}

		return data_table->GetFieldNameAsImmutableStr(exact_index);
	}

	size_t size() const override { return GetFieldsCount(); }

	inline size_t GetFieldsCount() const {

		return indexes.size();
	}

	int GetFieldsSizesSumm() const override {

		return summ;
	}

	virtual ~CFieldsProperties() { }
};
