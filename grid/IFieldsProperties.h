#pragma once
#include <basic/tstring.h>
#include "ISizes.h"

struct IFieldsProperties : public ISizes {
	virtual ImmutableString<Tchar> GetFieldName(const size_t index) const = 0;
	virtual int GetFieldsSizesSumm() const = 0;
	virtual void MultiplyAllSizesBy(const int multiplier) = 0;
	virtual void SetNewFieldMultiplier(const int multiplier) = 0;
	virtual ~IFieldsProperties() { }
};
