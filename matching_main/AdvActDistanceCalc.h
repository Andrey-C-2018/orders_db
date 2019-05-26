#pragma once
#include "IDistanceCalculator.h"

class CAdvActDistanceCalc : public IDistanceCalculator {
	enum{
		MAX_ACT_NAME_LEN = 30
	};

	struct Normalized {
		char act1[MAX_ACT_NAME_LEN + 1];
		char act2[MAX_ACT_NAME_LEN + 1];
		size_t len1;
		size_t len2;
	};
	Normalized normalize(const char *str1, const char *str2) const;
public:
	DistanceType get_distance(const char *str1, const char *str2) const override;
	bool is_equal(const char *str1, const char *str2) const override;
	virtual ~CAdvActDistanceCalc() { }
};

