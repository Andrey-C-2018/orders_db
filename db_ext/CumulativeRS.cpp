#include "CumulativeRS.h"

CumulativeRS::CumulativeRS(std::shared_ptr<IDbStatement> stmt_, \
							std::shared_ptr<IDbStatement> stmt_rec_count_) : \
						stmt(stmt_), stmt_rec_count(stmt_rec_count_),
						curr_page(0) {

	assert(stmt->getParamsCount() >= 2);

	rec_count = stmt_rec_count->execScalar();
	param_limit_size = stmt->getParamsCount() - 2;

	setCurrPageParams(0);
	rs = stmt->exec();
}
