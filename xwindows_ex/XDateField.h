#pragma once
#include "XTabStopWidget.h"
#include <date/Date.h>
#include "XDateCtrlManager.h"

class XDateField : public XTabStopWidget<XEdit> {
	XDateCtrlManager date_manager;
	
	inline void CreateInternal(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height);
	void OnChange(XCommandEvent *eve);

public:
	XDateField(ITabStopManager *manager_) noexcept;

	XDateField(ITabStopManager *manager_, XWindow *parent, \
				const int flags, const Tchar *label, \
				const int x, const int y, \
				const int width, const int height);

	XDateField(XDateField &&obj) = default;
	XDateField(const XDateField &obj) = delete;
	XDateField &operator=(const XDateField &obj) = delete;
	XDateField &operator=(XDateField &&obj) = default;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	virtual ~XDateField();
};

void XDateField::CreateInternal(XWindow *parent, const int flags, \
								const Tchar *label, \
								const int x, const int y, \
								const int width, const int height) {

	XTabStopWidget::Create(parent, flags, label, x, y, width, height);
	GetParent()->Connect(EVT_COMMAND, NCODE_EDIT_CHANGED, \
							GetId(), this, &XDateField::OnChange);
	XEdit::SetEditMaxLength(CDate::GERMAN_FORMAT_LEN);
}