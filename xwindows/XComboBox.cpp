#include "XComboBox.h"

XComboBox::XComboBox() : items_count(0){

CheckAndInitWindowClassName(_T("COMBOBOX"));
}

XComboBox::XComboBox(XWindow *parent, const int flags, \
			const Tchar *label, \
			const int x, const int y, \
			const int width, const int height){

CheckAndInitWindowClassName(_T("COMBOBOX"));
Create(parent, flags, label, x, y, width, height);
}

void XComboBox::AddItem(const Tchar *item){

assert(item);
_plComboBoxAddItem(GetInternalHandle(), item);
++items_count;
}

void XComboBox::DeleteItem(const size_t index) {

_plComboBoxDelItem(GetInternalHandle(), index);
--items_count;
}

size_t XComboBox::GetCurrentSelectionIndex() const{

return _plComboBoxGetCurrSel(GetInternalHandle());
}

void XComboBox::SetSelectionIndex(const size_t index) {

assert(index < items_count || index == EMPTY_INDEX);
_plComboBoxSetSel(GetInternalHandle(), index);
}

void XComboBox::GetItemText(const size_t index, Tstring item_text) const{
size_t len = _plComboBoxGetItemTextLen(GetInternalHandle(), index);
Tchar *text = nullptr;

_plComboBoxGetItemText(GetInternalHandle(), index, text);
item_text = text;
}

void XComboBox::Reset(){

_plComboBoxReset(GetInternalHandle());
}

XComboBox::~XComboBox(){ }
