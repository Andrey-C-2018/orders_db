#pragma once

struct IDbField;

struct IDbResultSetMetadata {
	virtual size_t getFieldsCount() const = 0;
	virtual std::shared_ptr<IDbField> getField(const size_t field) const = 0;
	virtual ~IDbResultSetMetadata(){ }
};