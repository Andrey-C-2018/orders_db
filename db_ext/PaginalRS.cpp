#include "PaginalRS.h"

PaginalRS::PaginalRS(std::shared_ptr<IDbStatement> stmt_, \
							std::shared_ptr<IDbStatement> stmt_rec_count_) : \
						stmt(stmt_), stmt_rec_count(stmt_rec_count_),
						curr_page(0) {

	assert(stmt->getParamsCount() >= 2);

	rec_count = stmt_rec_count->execScalar();
	param_limit_size = stmt->getParamsCount() - 2;

	stmt->bindValue(param_limit_size, (int)PAGE_SIZE);
	stmt->bindValue(param_limit_size + 1, (int)curr_page);
	rs = stmt->exec();
}
