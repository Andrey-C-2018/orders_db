#pragma once

class XGC;

struct IGCParamsList {

	virtual void Init(XGC &gc) = 0;
	virtual void Release() = 0;
	virtual ~IGCParamsList() { }
};