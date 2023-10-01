#include "DbInsertHelper.h"

CDbInsertHelper::CDbInsertHelper() : fields_count((size_t)-1) { }

CDbInsertHelper::CDbInsertHelper(const size_t fields_count_) : \
									fields_count(fields_count_) { }

void CDbInsertHelper::addBinder(const size_t field_no, \
							const Tchar *field_name, \
							std::shared_ptr<IInsertBinder> binder, \
							const size_t affected_params_count) {

	assert(this->fields_count != (size_t)-1);
	assert(binders.size() <= fields_count);
	assert(field_name);

	Binders item;
	item.field_no = field_no;
	item.field_name = field_name;
	item.binder = binder;
	item.affected_params_count = affected_params_count;

	auto p = std::lower_bound(binders.begin(), binders.end(), item);
	binders.insert(p, item);
}

void CDbInsertHelper::defStaticInsertion(const size_t field_no, const char *expr) {

	assert(this->fields_count != (size_t)-1);
	assert(static_insertions.size() <= fields_count);
	assert(expr);
	static_insertions[field_no] = std::string(expr);
}

void CDbInsertHelper::buildQuery(std::string &query) const {

	assert(this->fields_count != (size_t)-1);
	const size_t QUERY_RESERVED_SIZE = 70;
	auto appendToStrNTimes = [](std::string &str1, const char *str2, const size_t times) {

		for (size_t i = 0; i < times; i++)	str1 += str2;
	};

	assert(binders.size() + static_insertions.size() <= fields_count);
	query.reserve(QUERY_RESERVED_SIZE);

	auto p = static_insertions.cbegin();
	size_t i = 0, fields = 0;
	while (p != static_insertions.cend()) {

		while (i < binders.size() && binders[i].field_no < p->first) {
			appendToStrNTimes(query, "?,", binders[i].affected_params_count);
			fields += binders[i].affected_params_count;
			++i;
		}
		assert(i == binders.size() || binders[i].field_no != p->first);

		query += p->second;
		query += ',';

		++p; ++fields;
	}

	while (i < binders.size()) {
		appendToStrNTimes(query, "?,", binders[i].affected_params_count);
		fields += binders[i].affected_params_count;
		++i;
	}
	assert(fields == fields_count);
	query.erase(query.end() - 1);
}

CDbInsertHelper::~CDbInsertHelper() { }
