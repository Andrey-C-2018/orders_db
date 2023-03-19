#include "StaticResultSet.h"

StaticResultSet::StaticResultSet() : fields_count((size_t)-1), curr_record((size_t)-1) { }

void StaticResultSet::init(size_t fields_count_, size_t records_count) {

	assert (fields_count == (size_t)-1);
	assert (fields_count_ != 0 || (fields_count_ == 0 && records_count == 0));

	fields_count = fields_count_;
	if (fields_count == 0) return;

	std::vector<Variant> record_data;
	record_data.resize(fields_count);

	data.reserve(records_count);
	data.insert(data.end(), records_count, record_data);
}

void StaticResultSet::setValue(size_t field, size_t record, Variant value) {

	assert(field < fields_count);
	assert(record < data.size());
	auto &data_item = data[record][field];

	data_item = std::move(value);
}

StaticResultSet::~StaticResultSet() { }