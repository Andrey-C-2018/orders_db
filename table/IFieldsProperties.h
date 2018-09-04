#pragma once
#include "ISizes.h"
#include <basic/tstring.h>
#include <basic/ImmutableString.h>

struct IFieldsProperties : public ISizes {
	virtual ImmutableString<Tchar> GetName(const size_t index) const = 0;
	virtual int GetSizesSumm() const = 0;
	virtual ~IFieldsProperties() { }
};
