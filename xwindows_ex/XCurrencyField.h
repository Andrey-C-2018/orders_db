#pragma once
#include "XTabStopWidget.h"
#include "XCtrlInputFilter.h"

class XCurrencyField : public XTabStopWidget<XEdit> {
	XCtrlInputFilter filter;

	inline void CreateInternal(XWindow *parent, const int flags, \
								const Tchar *label, \
								const int x, const int y, \
								const int width, const int height);
	void OnChange(XCommandEvent *eve);

public:
	XCurrencyField(ITabStopManager *manager_) noexcept;

	XCurrencyField(ITabStopManager *manager_, XWindow *parent, \
					const int flags, const Tchar *label, \
					const int x, const int y, \
					const int width, const int height);

	XCurrencyField(XCurrencyField &&obj) = default;
	XCurrencyField(const XCurrencyField &obj) = delete;
	XCurrencyField &operator=(const XCurrencyField &obj) = delete;
	XCurrencyField &operator=(XCurrencyField &&obj) = default;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	virtual ~XCurrencyField();
};

//*****************************************************

void XCurrencyField::CreateInternal(XWindow *parent, const int flags, \
									const Tchar *label, \
									const int x, const int y, \
									const int width, const int height) {

	XTabStopWidget::Create(parent, flags, label, x, y, width, height);
	GetParent()->Connect(EVT_COMMAND, NCODE_EDIT_CHANGED, \
							GetId(), this, &XCurrencyField::OnChange);
}