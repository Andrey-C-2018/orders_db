#pragma once
#include "XWidget.h"

class XComboBox : public XWidget{
	size_t items_count;

	XComboBox(const XComboBox &obj) = delete;
	XComboBox &operator=(const XComboBox &obj) = delete;
public:
	enum {
		EMPTY_INDEX = (size_t) -1
	};

	XComboBox();
	XComboBox(XComboBox &&obj) = default;
	XComboBox &operator=(XComboBox &&obj) = default;
	XComboBox(XWindow *parent, const int flags, \
			const Tchar *label, \
			const int x, const int y, \
			const int width, const int height);
	void AddItem(const Tchar *item);
	void DeleteItem(const size_t index);
	size_t GetCurrentSelectionIndex() const;
	void SetSelectionIndex(const size_t index);
	void GetItemText(const size_t index, Tstring item_text) const;
	void Reset();
	virtual ~XComboBox();
};
