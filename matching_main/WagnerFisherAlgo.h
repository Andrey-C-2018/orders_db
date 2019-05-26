#pragma once
#include <assert.h>

inline size_t getWagnerFisherDistance(const char *str1, const size_t len1, \
									const char *str2, const size_t len2) {
assert(str1);
assert(str2);
	
const size_t SMALL_STRINGS_LEN = 25;
size_t *D = nullptr, D_arr[SMALL_STRINGS_LEN + 1], min;
size_t i, j;

bool release_D;
if ((len2 + 1) * 2 < SMALL_STRINGS_LEN) {
	D = &D_arr[0];
	D[SMALL_STRINGS_LEN] = '\0';
	release_D = false;
}
else {
	D = new size_t[(len2 + 1) * 2];
	release_D = true;
}

for (i = 0; i < len2 + 1; i++)
	D[2 * i] = i;

for (j = 1; j < (len1 + 1); j++) {
	D[1] = j;
	for (i = 1; i < (len2 + 1); i++) {
		if (str1[j - 1] == str2[i - 1])
			D[2 * i + 1] = D[2 * (i - 1)];
		else {
			min = (D[2 * i] < D[2 * i - 1]) ? D[2 * i] : D[2 * i - 1];
			min = (min >= D[2 * (i - 1)]) ? D[2 * (i - 1)] : min;
			D[2 * i + 1] = min + 1;
		}
		D[2 * (i - 1)] = D[2 * i - 1];
	}
	D[2 * len2] = D[2 * len2 + 1];
}

min = D[2 * len2 + 1];
if (release_D) delete[] D;

return min;
}
