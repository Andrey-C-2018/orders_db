#pragma once
#include <db_ext/IBinder.h>

class XWidget;

class CIntWidgetBinderControl : public IBinder {
	XWidget *widget;
public:
	CIntWidgetBinderControl(XWidget *widget_);

	void bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override;

	virtual ~CIntWidgetBinderControl();
};

