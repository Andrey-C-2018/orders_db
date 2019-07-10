#include <basic/XConv.h>
#include "WagnerFisherAlgo.h"
#include "StandartDistanceCalcs.h"

DistanceType CStrLengthDiffCalc::get_distance(const char *str1, const char *str2) const{
size_t len1 = 0, len2 = 0;
	
if(str1) len1 = std::strlen(str1);
if(str2) len2 = std::strlen(str2);

return (DistanceType)(len1 < len2 ? len1 - len2 : len2 - len1);
}

bool CStrLengthDiffCalc::is_equal(const char *str1, const char *str2) const {

return strcmp(str1, str2) == 0;
}

//**************************************************************

DistanceType CLevensteinDistanceCalc::get_distance(const char *str1, const char *str2) const{
size_t len1 = DISTANCE_MAX_VALUE, len2 = DISTANCE_MAX_VALUE;

if(str1) len1 = std::strlen(str1);
if(str2) len2 = std::strlen(str2);
if(!str1 || !str2) return DISTANCE_MAX_VALUE;

return (DistanceType)getWagnerFisherDistance(str1, len1, str2, len2);
}

bool CLevensteinDistanceCalc::is_equal(const char *str1, const char *str2) const {

return strcmp(str1, str2) == 0;
}

//**************************************************************

DistanceType CArithmeticDiffCalc::get_distance(const char *str1, const char *str2) const{
int error1 = 0, error2 = 0;
int num1 = XConv::ToInt(str1, error1);
int num2 = XConv::ToInt(str2, error2);

return error1 || error2 ? DISTANCE_MAX_VALUE : \
		(num1 > num2 ? num1 - num2 : num2 - num1);
}

bool CArithmeticDiffCalc::is_equal(const char *str1, const char *str2) const {

return strcmp(str1, str2) == 0;
}