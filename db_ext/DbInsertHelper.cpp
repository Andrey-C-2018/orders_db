#include "DbInsertHelper.h"

CDbInsertHelper::CDbInsertHelper() : fields_count((size_t)-1) { }

CDbInsertHelper::CDbInsertHelper(const size_t fields_count_) : \
									fields_count(fields_count_) { }

void CDbInsertHelper::addBinder(const size_t field_no, \
							const Tchar *field_name, \
							std::shared_ptr<IInsertBinder> binder) {

	assert(this->fields_count != (size_t)-1);
	assert(binders.size() <= fields_count);
	assert(field_name);

	Binders item;
	item.field_no = field_no;
	item.field_name = field_name;
	item.binder = binder;

	auto p = std::lower_bound(binders.begin(), binders.end(), item);
	binders.insert(p, item);
}

void CDbInsertHelper::defStaticInsertion(const size_t field_no, const char *expr) {

	assert(this->fields_count != (size_t)-1);
	assert(binders.size() <= fields_count);
	assert(expr);
	static_insertions[field_no] = std::string(expr);
}

void CDbInsertHelper::buildQuery(std::string &query) const {

	assert(this->fields_count != (size_t)-1);
	const size_t QUERY_RESERVED_SIZE = 70;
	auto appendToStr = [](std::string &str1, const char *str2, const size_t times) {

		for (size_t i = 0; i < times; ++i)	str1 += str2;
	};

	assert(!binders.empty());
	assert(binders.size() + static_insertions.size() == fields_count);
	query.reserve(QUERY_RESERVED_SIZE);

	auto p = static_insertions.cbegin();
	size_t start = 0;
	Binders item;
	while (p != static_insertions.cend()) {

		item.field_no = p->first;
		size_t end = std::distance(binders.cbegin(), \
				std::lower_bound(binders.cbegin(), binders.cend(), item));
		
		assert(start <= end);
		appendToStr(query, "?,", end - start);
		query += p->second;
		query += ',';

		start = end + 1;
		++p;
	}

	start -= (start > fields_count);
	assert(start <= fields_count);
	appendToStr(query, "?,", fields_count - start);
	query.erase(query.end() - 1);
}

CDbInsertHelper::~CDbInsertHelper() { }
