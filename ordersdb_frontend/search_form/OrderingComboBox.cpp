#include "OrderingComboBox.h"
#include <db_ext/MetaInfo.h>
#include <db_ext/SortingManager.h>
#include <grid/Grid.h>

COrderingComboBox::COrderingComboBox(CSortingManager *manager_) : \
									manager(manager_) {
	
	assert(manager);
}

void COrderingComboBox::Create(XWindow *parent, const int flags, \
								const Tchar *label, \
								const int x, const int y, \
								const int width, const int height) {

	XComboBox::Create(parent, flags | FL_COMBOBOX_DROPDOWN, label, x, y, width, height);
	XComboBox::AddItem(_T("(0)"));

	parent->Connect(EVT_COMMAND, NCODE_COMBOBOX_SELCHANGED, \
					GetId(), this, &COrderingComboBox::OnItemChoosed);
}

void COrderingComboBox::OnItemChoosed(XCommandEvent *eve) {

	size_t sel_index = GetCurrentSelectionIndex();
	if (!sel_index) {
		manager->reset();
		return;
	}

	auto p = ordering_map.find(sel_index);
	if (p == ordering_map.end()) {
		XException e(0, _T("No such field ID: "));
		e << sel_index;
		throw e;
	}
	manager->sortByColumn(p->second);
}

void COrderingComboBox::addOrderingField(const char *field_name, \
										const CMetaInfo &meta_info, \
										const CGrid *grid) {

	assert(field_name);
	assert(XComboBox::IsCreated());
	assert(grid);

	size_t index = meta_info.getFieldIndexByName(field_name);
	size_t sz = ordering_map.size() + 1;
	ordering_map[sz] = index + 1;

	XComboBox::AddItem(grid->GetFieldLabelAbs(index));
}

COrderingComboBox::~COrderingComboBox() { }
