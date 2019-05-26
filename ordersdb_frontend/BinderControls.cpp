#include "BinderControls.h"
#include <basic/XConv.h>
#include <basic/Exception.h>
#include <db/IDbBindingTarget.h>
#include <xwindows/XWidget.h>
#include <assert.h>

CIntWidgetBinderControl::CIntWidgetBinderControl(XWidget *widget_) : widget(widget_) {

	assert(widget);
}

void CIntWidgetBinderControl::bind(std::shared_ptr<IDbBindingTarget> binding_target, \
									const size_t param_no) {

	int err = 0;
	auto text = widget->GetLabel();
	int i = XConv::ToInt(text, err);

	if (err) {
		XException e(0, _T("the binding value is not an integer: "));
		e << text << _T(" , param no: ") << param_no;
		throw e;
	}

	binding_target->bindValue(param_no, i);
}

CIntWidgetBinderControl::~CIntWidgetBinderControl() { }
