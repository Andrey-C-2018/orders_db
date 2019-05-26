#pragma once
#include "IDistanceCalculator.h"

class CCurrencyDiffCalc : public IDistanceCalculator {
public:
	DistanceType get_distance(const char *str1, const char *str2) const override;
	bool is_equal(const char *str1, const char *str2) const override;
	virtual ~CCurrencyDiffCalc(){ }
};
