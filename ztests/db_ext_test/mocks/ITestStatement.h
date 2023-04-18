#pragma once

struct ITestStatement {

	virtual void reloadRS() = 0;
	virtual ~ITestStatement() { }
};
