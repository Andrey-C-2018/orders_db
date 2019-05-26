#pragma once
#include "IDistanceCalculator.h"

class CStrLengthDiffCalc : public IDistanceCalculator {
public:
	DistanceType get_distance(const char *str1, const char *str2) const override;
	bool is_equal(const char *str1, const char *str2) const override;
	virtual ~CStrLengthDiffCalc(){ }
};

class CLevensteinDistanceCalc : public IDistanceCalculator {
public:
	DistanceType get_distance(const char *str1, const char *str2) const override;
	bool is_equal(const char *str1, const char *str2) const override;
	virtual ~CLevensteinDistanceCalc(){ }
};

class CArithmeticDiffCalc : public IDistanceCalculator {
public:
	DistanceType get_distance(const char *str1, const char *str2) const override;
	bool is_equal(const char *str1, const char *str2) const override;
	virtual ~CArithmeticDiffCalc(){ }
};
