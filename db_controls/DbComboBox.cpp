#include "DbComboBox.h"
#include <db/IDbResultSet.h>
#include <xwindows_ex/ITabStopManager.h>

CDbComboBox::CDbComboBox(std::shared_ptr<const IDbResultSet> result_set_, \
							const size_t field_to_display_, const size_t prim_key_) : \
					result_set(result_set_), field_to_display(field_to_display_), \
					sel_index((size_t)-1), prim_key(prim_key_), empty_value_added(0), \
					manager(nullptr), def_null_value(0) { }

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

	XComboBox::Create(parent, flags | FL_COMBOBOX_DROPDOWN, label, x, y, width, height);
	fillComboBox();

	parent->Connect(EVT_COMMAND, NCODE_COMBOBOX_SELCHANGED, \
						GetId(), this, &CDbComboBox::OnItemChoosed);
}

void CDbComboBox::OnItemChoosed(XCommandEvent *eve) {

	sel_index = GetCurrentSelectionIndex() - empty_value_added;
	OnItemChoosed();
}

void CDbComboBox::OnKeyPress(XKeyboardEvent *eve) {

	if (!manager || eve->GetKey() != X_VKEY_TAB) {
		eve->ExecuteDefaultEventAction(true);
		return;
	}
	manager->TabPressedOnControl(this);
}

void CDbComboBox::setTabStopManager(ITabStopManager *manager_) {

	assert(manager_);
	this->manager = manager_;
	manager->RegisterTabStopControl(this);

	XComboBox::OverrideWindowEvent(EVT_KEYDOWN, \
				XEventHandlerData(this, &CDbComboBox::OnKeyPress));
}

int CDbComboBox::getPrimaryKeyAsInteger() const {

	assert(prim_key != (size_t)-1);
	if (isEmptySelection())
		throw XException(0, _T("the DbComboBox item is not choosen or empty"));
	
	result_set->gotoRecord(sel_index);

	bool is_null;
	int prim_key_value = result_set->getInt(prim_key, is_null);
	assert(!is_null);

	return prim_key_value;
}

void CDbComboBox::SetCurrRecord(const size_t prim_key_value) {

	size_t records_count = result_set->getRecordsCount();
	bool Found = false, is_null;
	size_t i = 0;
	for (; i < records_count && !Found; ++i) {

		result_set->gotoRecord(i);
		Found = (result_set->getInt(prim_key, is_null) == prim_key_value);
	}
	if (!Found) {
		XException e(0, _T("DBComboBox: no such prim key value: "));
		e << prim_key_value;
		throw e;
	}

	--i;
	this->SetSelectionIndex(i + empty_value_added);
	sel_index = i;

	OnItemChoosed();
}

void CDbComboBox::SetCurrRecord(std::shared_ptr<const IDbResultSet> rs, \
								const size_t prim_key_no_in_rs, int prim_key_type_hint) {

	bool is_null;
	int value = rs->getInt(prim_key_no_in_rs, is_null);
	if (is_null) value = def_null_value;

	SetCurrRecord(value);
}

void CDbComboBox::selectEmptyValue() {

	this->SetSelectionIndex(0);
	sel_index = (size_t)-1;
	OnItemChoosed();
}

CDbComboBox::~CDbComboBox() { }
