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

void XComboBox::GetItemText(const size_t index, XString<Tchar> &item_text) const{

	GetItemTextInternal(index, item_text);
}

void XComboBox::GetCurrentItemText(XString<Tchar> &item_text) const {

	size_t curr_selection = _plComboBoxGetCurrSel(GetInternalHandle());
	GetItemTextInternal(curr_selection, item_text);
}

int XComboBox::OverrideWindowEvent(const _plEventId id_event, \
									XEventHandlerData evt_handler_data) {
	_plHWND hwnds[COMBOBOX_EXTRA_HWNDS_COUNT];
	
	_plGetComboBoxHWNDs(GetInternalHandle(), hwnds);
	assert(hwnds[0] == GetInternalHandle());

	int ret_value;
	ret_value = OverrideWindowEventImpl(id_event, evt_handler_data);
	for(size_t i = 1; i < COMBOBOX_EXTRA_HWNDS_COUNT; ++i)
		ret_value &= OverrideWindowExtraHwnd(id_event, hwnds[i], evt_handler_data);

	return ret_value;
}

void XComboBox::Reset(){

	_plComboBoxReset(GetInternalHandle());
}

XComboBox::~XComboBox(){ }
