#include "Query.h"

CQuery::CQuery(std::shared_ptr<IDbStatement> stmt_) : stmt(stmt_) { }

std::shared_ptr<IDbStatement> CQuery::getUpdateStmt(const size_t updated_field, \
									std::shared_ptr<IDbResultSet> result_set) const {


}

std::shared_ptr<IDbStatement> CQuery::getDeleteStmt(std::shared_ptr<IDbResultSet> result_set) const {


}

std::shared_ptr<IDbBindingTarget> CQuery::getBindingTarget() {

}

std::shared_ptr<IDbResultSet> CQuery::exec() {

}

CQuery::~CQuery(){ }