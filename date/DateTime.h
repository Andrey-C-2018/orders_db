#pragma once
#include "Date.h"

class CDateTime : public CDate {
	unsigned hours;
	unsigned minutes;
	unsigned seconds;

public:
	CDateTime();
	virtual ~CDateTime();
};

