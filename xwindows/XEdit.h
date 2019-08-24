#pragma once
#include "XWidget.h"

class XEdit : public XWidget{
public:
	XEdit();

	XEdit(const XEdit &obj) = delete;
	XEdit(XEdit &&obj) = default;
	XEdit &operator=(const XEdit &obj) = delete;
	XEdit &operator=(XEdit &&obj) = default;
	
	XEdit(XWindow *parent, const int flags, \
			const Tchar *label, \
			const int x, const int y, \
			const int width, const int height);

	void SetEditMaxLength(const size_t max_len);
	void SetSelection(const size_t begin, const size_t end);
	void SelectAll();

	virtual ~XEdit();
};