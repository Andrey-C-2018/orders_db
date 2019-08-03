#pragma once
#include <memory>
#include <ostream>
#include <assert.h>

class CMatchingResultsTable;

class CHTMLOutputWrapper{
	const CMatchingResultsTable &table;
public:
	CHTMLOutputWrapper(const CMatchingResultsTable &table_);
	CHTMLOutputWrapper(const CHTMLOutputWrapper &obj) = default;
	CHTMLOutputWrapper(CHTMLOutputWrapper &&obj) = default;
	CHTMLOutputWrapper &operator=(const CHTMLOutputWrapper &obj) = default;
	CHTMLOutputWrapper &operator=(CHTMLOutputWrapper &&obj) = default;

	std::ostream &writeHeader(std::ostream &out);
	friend std::ostream &operator<<(std::ostream &out, const CHTMLOutputWrapper &obj);
	std::ostream &writeTail(std::ostream &out);
	
	virtual ~CHTMLOutputWrapper();
};

class CCsvOutputWrapper {
	const CMatchingResultsTable &table;
	std::string prefix;

	inline std::ostream &writeCSValue(const char *value, std::ostream &out) const;
public:
	CCsvOutputWrapper(const CMatchingResultsTable &table_);
	CCsvOutputWrapper(const CMatchingResultsTable &table_, std::string prefix_);

	CCsvOutputWrapper(const CCsvOutputWrapper &obj) = default;
	CCsvOutputWrapper(CCsvOutputWrapper &&obj) = default;
	CCsvOutputWrapper &operator=(const CCsvOutputWrapper &obj) = default;
	CCsvOutputWrapper &operator=(CCsvOutputWrapper &&obj) = default;

	std::ostream &writeHeader(std::ostream &out);
	friend std::ostream &operator<<(std::ostream &out, const CCsvOutputWrapper &obj);
	std::ostream &writeTail(std::ostream &out);

	virtual ~CCsvOutputWrapper();
};

std::ostream &CCsvOutputWrapper::writeCSValue(const char *value, std::ostream &out) const{

	if (!value) {
		out << "NULL";
		return out;
	}

	auto p = strchr(value, ',');
	if (p) out << '"' << value << '"';
	else out << value;

	return out;
}