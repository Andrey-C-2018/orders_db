#pragma once

struct ISizes {
	virtual int GetWidth(const size_t index) const = 0;
	virtual size_t size() const = 0;
	virtual ~ISizes() { }
};
