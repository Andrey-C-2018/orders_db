#include "DbComboBox.h"
#include <db/IDbResultSet.h>

CDbComboBox::CDbComboBox(std::shared_ptr<const IDbResultSet> result_set_, \
							const size_t field_to_display_) : \
					result_set(result_set_), field_to_display(field_to_display_) { }

void CDbComboBox::fillComboBox() {

	size_t records_count = result_set->getRecordsCount();
	for (size_t i = 0; i < records_count; ++i) {
		result_set->gotoRecord(i);
		XComboBox::AddItem(result_set->getWString(field_to_display));
	}
}

void CDbComboBox::Create(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height) {

	XComboBox::Create(parent, flags, label, x, y, width, height);
	fillComboBox();

	parent->Connect(EVT_COMMAND, NCODE_COMBOBOX_SELCHANGED, \
						GetId(), this, &CDbComboBox::OnItemChoosed);
}

void CDbComboBox::OnItemChoosed(XCommandEvent *eve) {

	size_t sel_index = GetCurrentSelectionIndex();
	result_set->gotoRecord(sel_index);
}

CDbComboBox::~CDbComboBox() { }
