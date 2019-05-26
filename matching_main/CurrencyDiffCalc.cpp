#include "Currency.h"
#include "CurrencyDiffCalc.h"

DistanceType CCurrencyDiffCalc::get_distance(const char *str1, const char *str2) const{
CCurrency l, r;	

if(l.init(str1)) return DISTANCE_MAX_VALUE;
if(r.init(str2)) return DISTANCE_MAX_VALUE;

return l.getDifference(r);
}

bool CCurrencyDiffCalc::is_equal(const char *str1, const char *str2) const {
CCurrency l, r;

if (l.init(str1)) return DISTANCE_MAX_VALUE;
if (r.init(str2)) return DISTANCE_MAX_VALUE;

return l == r;
}