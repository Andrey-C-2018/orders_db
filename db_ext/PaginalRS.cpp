#include "PaginalRS.h"

PaginalRS::PaginalRS(std::shared_ptr<IDbStatement> stmt_, \
							std::shared_ptr<IDbStatement> stmt_rec_count_) : \
						stmt(stmt_), stmt_rec_count(stmt_rec_count_),
						curr_page_first_rec(0) {

	assert(stmt->getParamsCount() >= 2);

	rs_rec_count = stmt_rec_count->exec();
	rs_rec_count->gotoRecord(0);
	bool is_null;
	rec_count = rs_rec_count->getInt(0, is_null);
	param_limit_size = stmt->getParamsCount() - 2;

	stmt->bindValue(param_limit_size, (int)PAGE_SIZE);
	stmt->bindValue(param_limit_size + 1, (int)curr_page_first_rec);
	rs = stmt->exec();

	auto cloned = rs->staticClone();
	assert (cloned != nullptr);
	rs_cache.emplace(0, cloned);
	curr_rs = rs;
}
