#include <algorithm>
#include <basic/tstring.h>
#include "AdvActDistanceCalc.h"
#include "WagnerFisherAlgo.h"

CAdvActDistanceCalc::Normalized CAdvActDistanceCalc::normalize(const char *str1, const char *str2) const {
auto is_not_backspace = [](const char ch) -> bool {
	return ch != ' ';
};
Normalized r;
char *p = nullptr;

r.len1 = strlen(str1);
r.len2 = strlen(str2);

if (r.len1 > MAX_ACT_NAME_LEN || r.len2 > MAX_ACT_NAME_LEN) {
	r.len1 = (size_t)-1;
	r.len2 = (size_t)-1;
	return r;
}

p = std::copy_if(str1, str1 + r.len1, r.act1, is_not_backspace);
r.len1 = p - r.act1;
*p = '\0';

p = std::copy_if(str2, str2 + r.len2, r.act2, is_not_backspace);
r.len2 = p - r.act2;
*p = '\0';

return r;
}

DistanceType CAdvActDistanceCalc::get_distance(const char *str1, const char *str2) const {

if (str1 && str2) {
	Normalized r = normalize(str1, str2);

	if (r.len1 == (size_t)-1 && r.len2 == (size_t)-1)
		return DISTANCE_MAX_VALUE;
	return (DistanceType) getWagnerFisherDistance(r.act1, r.len1, r.act2, r.len2);
}
return DISTANCE_MAX_VALUE;
}

bool CAdvActDistanceCalc::is_equal(const char *str1, const char *str2) const {

if (!str1 || !str2) return false;

Normalized r = normalize(str1, str2);

if ((r.len1 == (size_t)-1 && r.len2 == (size_t)-1) || r.len1 != r.len2)
	return false;
return std::strncmp(r.act1, r.act2, r.len1) == 0;
}