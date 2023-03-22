#include "StaticResultSet.h"

StaticResultSet::StaticResultSet(size_t field_rec_id_, \
								std::unordered_set<int> &rec_ids_) : \
							fields_count((size_t)-1), \
							field_rec_id(field_rec_id_), \
							rec_ids(&rec_ids_), \
							is_double(false), \
							curr_record((size_t)-1) { }

void StaticResultSet::init(size_t fields_count_, size_t records_count) {

	assert (fields_count == (size_t)-1);
	assert (fields_count_ != 0 || (fields_count_ == 0 && records_count == 0));
	assert (fields_count_ == 0 || field_rec_id < fields_count_);

	fields_count = fields_count_;
	if (fields_count == 0) return;

	std::vector<Variant> record_data;
	record_data.resize(fields_count);

	data.reserve(records_count);
	data.insert(data.end(), records_count, record_data);
}

size_t StaticResultSet::getFieldRecId() const {

	return field_rec_id;
}

bool StaticResultSet::gotoRecord(size_t record, int field_id_value) {

	gotoRecord(record);
	auto p = rec_ids->insert(field_id_value);
	is_double = !p.second;
	return is_double;
}

void StaticResultSet::setValue(size_t field, Variant value) {

	assert(field < fields_count);
	if (!is_double) {

		auto &data_item = data[curr_record][field];
		data_item = std::move(value);
	}
}

StaticResultSet::~StaticResultSet() { }