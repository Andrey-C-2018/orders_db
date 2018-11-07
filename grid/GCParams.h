#pragma once
#include <xwindows/XGC.h>

struct IGCParams {
	virtual void SetParametersToGC(XGC *gc) = 0;
	virtual void RestoreDefaultGCParameters(XGC *gc) = 0;
	virtual ~IGCParams() { }
};

class CNullGCParams : public IGCParams {
public:
	void SetParametersToGC(XGC *gc) override { }
	void RestoreDefaultGCParameters(XGC *gc) override { }
	virtual ~CNullGCParams() { }
};

class CGCParams : public IGCParams {
	int old_bk_mode;
	XColor bg_color, old_bg_color;
	XFont *font, old_font;
public:
	CGCParams(XFont *font_, XColor background_color);

	void SetParametersToGC(XGC *gc) override;
	void RestoreDefaultGCParameters(XGC *gc) override;
	virtual ~CGCParams() { }
};