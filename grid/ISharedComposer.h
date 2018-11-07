#pragma once
#include "RecordComposer.h"

struct ISharedComposerContainer {
	virtual const CRecordComposer getComposer() const = 0;
	virtual ~ISharedComposerContainer() { }
};