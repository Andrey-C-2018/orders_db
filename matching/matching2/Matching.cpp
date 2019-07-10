#include <list>
#include <iostream>
#include "Matching.h"

CMatchingException::CMatchingException(const int err_code, \
									const Tchar *err_descr) : \
									XException(err_code, err_descr) { }

CMatchingException::CMatchingException(const CMatchingException &obj) : \
									XException(obj) { }

CMatchingException::~CMatchingException() { }

//**************************************************************

CMatching::CMatching(std::shared_ptr<ITable> in1_, \
					std::shared_ptr<ITable> in2_) : \
					in1(in1_), in2(in2_){
		       
assert(in1);
assert(in2);
}

void CMatching::addDistanceCalculator(const char *column_name_in1, \
									const char *column_name_in2, \
									std::shared_ptr<IDistanceCalculator> calculator){
CDistanceCalculatorItem calc_item;

assert(column_name_in1);
assert(column_name_in2);
assert(calculator);

calc_item.input1_col_index = in1->getColIndexByName(column_name_in1);
if(calc_item.input1_col_index == ITable::NOT_FOUND){
	CMatchingException e(CMatchingException::E_WRONG_COL_NAME, \
								_T("The column name is not found: "));
	e<< column_name_in1;
	throw e;
}
calc_item.input2_col_index = in2->getColIndexByName(column_name_in2);
if(calc_item.input2_col_index == ITable::NOT_FOUND){
	CMatchingException e(CMatchingException::E_WRONG_COL_NAME, \
								_T("The column name is not found: "));
	e<< column_name_in2;
	throw e;
}
calc_item.calculator = calculator;

calculators.push_back(calc_item);
}

template <size_t N> \
void CMatching::diff(std::shared_ptr<const IRecord> rec1, \
						std::shared_ptr<const IRecord> rec2, DistanceType(&result)[N]) const{

assert(N == calculators.size());
size_t i = 0;
for(auto &p : calculators){
	const char *str1 = rec1->getColValueAsCharArray(p.input1_col_index);
	const char *str2 = rec2->getColValueAsCharArray(p.input2_col_index);

	*(result + i) = p.calculator->get_distance(str1, str2);
	++i;
}
}

CMatchingResultsTable CMatching::match(const size_t start_in1, const size_t end_in1, \
									const size_t start_in2, const size_t end_in2, \
									const bool include_header){

if(calculators.empty())
	throw CMatchingException(CMatchingException::E_NO_METRIC_CALCS_ADDED, \
							_T("The metric calculators are not added"));
assert(end_in1 <= in1->getRecordsCount());
assert(end_in2 <= in2->getRecordsCount());

std::vector<DistanceType> distances;
distances.insert(distances.begin(), calculators.size(), DISTANCE_MAX_VALUE);

CMatchingResultsTable matching_results(in1, in2, include_header);

std::list<size_t> in2_indexes;
for(size_t i = start_in2; i < end_in2; ++i) in2_indexes.push_back(i);

for(size_t k = start_in1; k < end_in1; ++k){
	std::shared_ptr<const IRecord> rec = in1->getRecord(k);

	size_t index = CMatchingResultsTable::NULL_INDEX;
	auto p_used_index = in2_indexes.cbegin();
	for(auto p_index_in2 = in2_indexes.cbegin(); \
		p_index_in2 != in2_indexes.cend(); \
		++p_index_in2){
		
		std::shared_ptr<const IRecord> rec2 = in2->getRecord(*p_index_in2);
	
		size_t j = 0;
		DistanceType distance_backup;
		bool is_less_cumulative = false;
		bool new_min_distance_found;
		auto p = calculators.begin();
		do{
			const char *str1 = nullptr, *str2 = nullptr;
			str1 = rec->getColValueAsCharArray(p->input1_col_index);
			str2 = rec2->getColValueAsCharArray(p->input2_col_index);
				
			DistanceType m = p->calculator->get_distance(str1, str2);
			
			bool is_equal, is_bigger;
			is_less_cumulative = is_less_cumulative || (m < distances[j]);
			is_equal = (m == distances[j]);
			is_bigger = (m > distances[j]);
				
			distance_backup = distances[j];
			distances[j] = m;
			++j;
			++p;
		
			new_min_distance_found = (is_equal || is_less_cumulative) && \
									(!is_bigger || is_less_cumulative);
		} while(new_min_distance_found && p != calculators.end());
	
		if(!new_min_distance_found){
			--j;
			distances[j] = distance_backup;
		}
		else{
			index = *p_index_in2;
			p_used_index = p_index_in2;
		}
	}

	if(index != CMatchingResultsTable::NULL_INDEX)
		in2_indexes.erase(p_used_index);

	matching_results.addIndexes(k, index);
	std::fill(distances.begin(), distances.end(), DISTANCE_MAX_VALUE);
}

return matching_results;
}

//**************************************************************

CEqualMatching::CEqualMatching(std::shared_ptr<ITable> in1_, \
								std::shared_ptr<ITable> in2_) : CMatching(in1_, in2_) { } 

CMatchingResultsTable CEqualMatching::match(const size_t start_in1, const size_t end_in1, \
											const size_t start_in2, const size_t end_in2, \
											const bool include_header) {
assert(end_in1 <= in1->getRecordsCount());
assert(end_in2 <= in2->getRecordsCount());

CMatchingResultsTable matching_results(in1, in2, include_header);
std::list<size_t> excluded1, excluded2;

for (size_t i = start_in1; i < end_in1; ++i)
	excluded1.push_back(i);

for (size_t i = start_in2; i < end_in2; ++i)
	excluded2.push_back(i);

for (auto p = excluded1.cbegin(); p != excluded1.cend() && excluded2.size(); ) {
	std::shared_ptr<const IRecord> rec = in1->getRecord(*p);
	bool inc = true;

	for (auto p2 = excluded2.begin(); p2 != excluded2.end(); ++p2) {
		std::shared_ptr<const IRecord> rec2 = in2->getRecord(*p2);

		bool equal = true;
		for (size_t i = 0; i < getCalculatorsCount(); ++i) {

			const char *str1 = nullptr, *str2 = nullptr;
			const CDistanceCalculatorItem &calc = getCalculatorItem(i);
			str1 = rec->getColValueAsCharArray(calc.input1_col_index);
			str2 = rec2->getColValueAsCharArray(calc.input2_col_index);

			equal = equal && calc.calculator->is_equal(str1, str2);
		}
		if (equal && getCalculatorsCount()) {
			matching_results.addIndexes(*p, *p2);

			auto p2_to_erase = p2; ++p2;
			excluded2.erase(p2_to_erase);

			auto p_to_erase = p; ++p;
			excluded1.erase(p_to_erase);
			inc = false;

			break;
		}
	}

	if(inc) ++p;
}

for (auto index1 : excluded1)
	matching_results.addIndexes(index1, CMatchingResultsTable::NULL_INDEX);
for (auto index2 : excluded2)
	matching_results.addIndexes(CMatchingResultsTable::NULL_INDEX, index2);

return matching_results;
}
