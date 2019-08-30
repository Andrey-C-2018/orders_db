#pragma once
#include <xwindows/XEdit.h>
#include "XTabStopWidget.h"

class CEditTabStopAction : public ITabStopAction {
	XEdit *edit;
public:
	CEditTabStopAction(XEdit *edit_) : edit(edit_) { }

	CEditTabStopAction(const CEditTabStopAction &obj) = default;
	CEditTabStopAction(CEditTabStopAction &&obj) = default;
	CEditTabStopAction &operator=(const CEditTabStopAction &obj) = default;
	CEditTabStopAction &operator=(CEditTabStopAction &&obj) = default;

	void exec(XWidget *widget) override {

		assert(edit == widget);
		edit->SelectAll();
	}

	virtual ~CEditTabStopAction() { }
};

class XTabStopEdit : public XTabStopWidget<XEdit> {
	CEditTabStopAction edit_action;
public:
	XTabStopEdit(ITabStopManager *manager_) noexcept;

	XTabStopEdit(ITabStopManager *manager_, XWindow *parent, \
				const int flags, const Tchar *label, \
				const int x, const int y, \
				const int width, const int height);

	XTabStopEdit(const XTabStopEdit &obj) = delete;
	XTabStopEdit(XTabStopEdit &&obj) = default;
	XTabStopEdit &operator=(const XTabStopEdit &obj) = delete;
	XTabStopEdit &operator=(XTabStopEdit &&obj) = default;

	virtual ~XTabStopEdit();
};