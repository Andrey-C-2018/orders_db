#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <basic/Exception.h>
#include "IInsertBinder.h"

struct IDbConnection;

class CDbInsertHelper {
	size_t fields_count;

	struct Binders {
		size_t field_no;
		const Tchar *field_name;
		std::shared_ptr<IInsertBinder> binder;

		inline bool operator<(const Binders &obj) const { 
			
			return field_no < obj.field_no; 
		}
	};
	std::vector<Binders> binders;
	std::map<size_t, std::string> static_insertions;

public:
	CDbInsertHelper();
	CDbInsertHelper(const size_t fields_count_);

	inline void init(const size_t fields_count_);

	CDbInsertHelper(const CDbInsertHelper &obj) = default;
	CDbInsertHelper(CDbInsertHelper &&obj) = default;
	CDbInsertHelper &operator=(const CDbInsertHelper &obj) = default;
	CDbInsertHelper &operator=(CDbInsertHelper &&obj) = default;

	void addBinder(const size_t field_no, const Tchar *field_name, \
							std::shared_ptr<IInsertBinder> binder);
	void defStaticInsertion(const size_t field_no, const char *expr);
	inline size_t getBindersCount() const;

	void buildQuery(std::string &query) const;
	inline bool bind(std::shared_ptr<IDbBindingTarget> stmt, \
						const size_t initial_param_no, Tstring &err_str) const;

	virtual ~CDbInsertHelper();
};

//*****************************************************

void CDbInsertHelper::init(const size_t fields_count_) {

	assert(this->fields_count == (size_t)-1);
	this->fields_count = fields_count_;
}

size_t CDbInsertHelper::getBindersCount() const {

	return binders.size();
}

bool CDbInsertHelper::bind(std::shared_ptr<IDbBindingTarget> stmt, \
							const size_t initial_param_no, Tstring &err_str) const {

	IInsertBinder::Params params;
	params.param_no = initial_param_no;

	bool cancel = false;
	for (auto p_item = binders.begin(); !cancel && p_item != binders.end(); ++p_item) {
		auto param_no = params.param_no;
		auto err_str_size = params.error_str.size();

		cancel = !p_item->binder->bind(stmt, params, p_item->field_name);
		assert(param_no < params.param_no);
		assert(params.param_no <= initial_param_no + fields_count);
		assert(err_str_size <= params.error_str.size());
	}

	if (!params.error_str.empty() && err_str.empty())
		err_str = std::move(params.error_str);
	else
		err_str += params.error_str;

	return !cancel && err_str.empty();
}
