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
		summ = 0;

		if (!fields_count)
			fields_props.reserve(DEF_FIELDS_COUNT);
		else {
			fields_props.reserve(fields_count);

			std::vector<Tchar> empty_field_name;
			for (size_t i = 0; i < fields_count; ++i) {
				int field_size = (int)data_table->GetFieldMaxLengthInChars(i);

				field_size *= multiplier;

				fields_props.push_back(FieldProps{ field_size, empty_field_name, true });
				summ += field_size;
			}
		}
	}

	int GetWidth(const size_t index) const override {

		assert(index < fields_props.size());
		return fields_props[index].field_size;
	}

	inline void SetWidth(const size_t index, const int field_width) {

		assert(index < fields_props.size());
		summ -= fields_props[index].field_size;
		summ += field_width;

		fields_props[index].field_size = field_width;
	}

	inline void HideField(const size_t index) {

		assert(index < fields_props.size());

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

		assert(index < fields_props.size());
		assert(data_table);
		if (fields_props[index].field_name.size()) {
			ImmutableString<Tchar> field_name(&fields_props[index].field_name[0], \
												fields_props[index].field_name.size());
			return field_name;
		}

		return data_table->GetFieldNameAsImmutableStr(index);
	}

	size_t size() const override { return GetFieldsCount(); }

	inline size_t GetFieldsCount() const {

		return fields_props.size();
	}

	int GetFieldsSizesSumm() const override {

		return summ;
	}

	virtual ~CFieldsProperties() { }
};
