#pragma once

struct IReloadable {
	virtual void Reload() = 0;
	virtual ~IReloadable() { }
};
