#include "PaginalRS.h"

PaginalRS::PaginalRS(std::shared_ptr<IDbStatement> stmt_, \
							std::shared_ptr<IDbStatement> stmt_rec_count_,\
							size_t field_id_index_) : \
					 	page_size(DEF_PAGE_SIZE), \
					 	param_limit_size((size_t)-1), \
					 	field_id_index(field_id_index_), \
						rec_count(0), \
						stmt(stmt_), stmt_rec_count(stmt_rec_count_), \
						curr_page_first_rec(0), \
						last_binded_page ((size_t)-1) {

	init();
}

PaginalRS::PaginalRS(size_t page_size_, \
					std::shared_ptr<IDbStatement> stmt_,
					 std::shared_ptr<IDbStatement> stmt_rec_count_, \
					 size_t field_id_index_) : \
					 	page_size(page_size_), \
					 	param_limit_size((size_t)-1), \
					 	field_id_index(field_id_index_), \
					 	rec_count(0), \
						stmt(stmt_), stmt_rec_count(stmt_rec_count_), \
						curr_page_first_rec(0), \
						last_binded_page ((size_t)-1) {

	init();
}

void PaginalRS::init() {

	assert (param_limit_size == (size_t)-1);
	assert (stmt->getParamsCount() >= 2);

	rs_rec_count = stmt_rec_count->exec();
	rs_rec_count->gotoRecord(0);
	bool is_null;
	rec_count = rs_rec_count->getInt(0, is_null);
	param_limit_size = stmt->getParamsCount() - 2;

	stmt->bindValue(param_limit_size, (int)page_size);
	stmt->bindValue(param_limit_size + 1, (int)curr_page_first_rec);
	last_binded_page = curr_page_first_rec;
	rs = stmt->exec();

	addRSToCache();
}

bool PaginalRS::empty() const {

	return rec_count == 0;
}

size_t PaginalRS::getFieldsCount() const {

	return rs->getFieldsCount();
}

size_t PaginalRS::getRecordsCount() const {

	return rec_count;
}

void PaginalRS::gotoRecord(const size_t record) const {

	assert(record < rec_count);
	if (curr_page_first_rec > record || record >= curr_page_first_rec + page_size) {

		curr_page_first_rec = (record / page_size) * page_size;
		auto p = rs_cache.find(curr_page_first_rec);
		if (p != rs_cache.end())
			curr_rs = p->second;
		else {

			stmt->bindValue(param_limit_size, (int) page_size);
			stmt->bindValue(param_limit_size + 1, (int) curr_page_first_rec);
			last_binded_page = curr_page_first_rec;
			rs->reload();

			addRSToCache();
		}
	}

	curr_rs->gotoRecord(record % page_size);
}

int PaginalRS::getInt(const size_t field, bool &is_null) const {

	return curr_rs->getInt(field, is_null);
}

void PaginalRS::reload() {

	rs_rec_count->reload();
	rs_rec_count->gotoRecord(0);
	bool is_null;
	rec_count = rs_rec_count->getInt(0, is_null);

	if (rec_count <= curr_page_first_rec || last_binded_page != curr_page_first_rec) {
		curr_page_first_rec = rec_count % page_size == 0 ? \
							(rec_count >= page_size ? rec_count - page_size : 0) : \
							(rec_count / page_size) * page_size;

		stmt->bindValue(param_limit_size, (int)page_size);
		stmt->bindValue(param_limit_size + 1, (int)curr_page_first_rec);
		last_binded_page = curr_page_first_rec;
	}
	rs->reload();
	rs_cache.clear();
	rec_ids.clear();

	addRSToCache();
}

const char *PaginalRS::getString(const size_t field) const {

	return curr_rs->getString(field);
}

const wchar_t *PaginalRS::getWString(const size_t field) const {

	return curr_rs->getWString(field);
}

ImmutableString<char> PaginalRS::getImmutableString(const size_t field) const {

	return curr_rs->getImmutableString(field);
}

ImmutableString<wchar_t> PaginalRS::getImmutableWString(const size_t field) const {

	return curr_rs->getImmutableWString(field);
}

CDate PaginalRS::getDate(const size_t field, bool &is_null) const {

	return curr_rs->getDate(field, is_null);
}

PaginalRS::~PaginalRS() { }
