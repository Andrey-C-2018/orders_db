#pragma once
#include <memory>
#include <vector>
#include <list>
#include <basic/Exception.h>
#include "MatchingResultsTable.h"
#include "IDistanceCalculator.h"
#include "ITable.h"

class CMatchingException : public XException{
public:
	enum Errors{
		E_COL_NAME_IS_NULL = 1, \
		E_WRONG_COL_NAME = 2, \
		E_NO_METRIC_CALCS_ADDED = 3
	};
	
	CMatchingException(const int err_code, const Tchar *err_descr);
	CMatchingException(const CMatchingException &obj);
	CMatchingException(CMatchingException &&obj) = default;
	virtual ~CMatchingException();
};

class CMatching{
protected:
	struct CDistanceCalculatorItem {
		size_t input1_col_index;
		size_t input2_col_index;
		std::shared_ptr<IDistanceCalculator> calculator;
	};

	std::shared_ptr<ITable> in1, in2;
	inline size_t getCalculatorsCount() const;
	inline const CDistanceCalculatorItem &getCalculatorItem(const size_t index) const;

	template <size_t N> \
		void diff(std::shared_ptr<const IRecord> rec1, \
					std::shared_ptr<const IRecord> rec2, DistanceType (&result)[N]) const;	
private:
	std::vector<CDistanceCalculatorItem> calculators;
public:
	CMatching(std::shared_ptr<ITable> in1_, \
				std::shared_ptr<ITable> in2_);

	void addDistanceCalculator(const char *column_name_in1, \
								const char *column_name_in2, \
								std::shared_ptr<IDistanceCalculator> calculator);

	virtual CMatchingResultsTable match(const size_t start_in1, const size_t end_in1, \
								const size_t start_in2, const size_t end_in2, \
								const bool include_header);
	virtual ~CMatching(){ }
};

//**************************************************************

class CEqualMatching : public CMatching {
public:
	CEqualMatching(std::shared_ptr<ITable> in1_, \
					std::shared_ptr<ITable> in2_);
	CMatchingResultsTable match(const size_t start_in1, const size_t end_in1, \
								const size_t start_in2, const size_t end_in2, \
								const bool include_header) override;
	virtual ~CEqualMatching() { }
};

//**************************************************************

size_t CMatching::getCalculatorsCount() const {

return calculators.size();
}

const CMatching::CDistanceCalculatorItem &CMatching::getCalculatorItem(const size_t index) const {

assert(index < calculators.size());
return calculators[index];
}
