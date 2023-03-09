#pragma once
#include <memory>
#include "IDbBindingTarget.h"
#include "IDbCommon.h"

struct IDbResultSet;
struct IDbResultSetMetadata;

struct IDbStatement : public IDbBindingTarget {
	virtual std::shared_ptr<IDbResultSet> exec() = 0;
	virtual record_t execScalar() = 0;
	virtual std::shared_ptr<IDbResultSetMetadata> getResultSetMetadata() = 0;
        virtual size_t getParamsCount() const = 0;
	virtual ~IDbStatement() { }
};
