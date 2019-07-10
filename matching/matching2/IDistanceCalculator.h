#pragma once
#include <climits>

typedef size_t DistanceType;
const DistanceType DISTANCE_MAX_VALUE = SIZE_MAX;

struct IDistanceCalculator{
	virtual DistanceType get_distance(const char *str1, const char *str2) const = 0;
	virtual bool is_equal(const char *str1, const char *str2) const = 0;
	virtual ~IDistanceCalculator(){ }
};