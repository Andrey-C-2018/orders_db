#pragma once
#include <xwindows/XComboBox.h>

struct IDbResultSet;
struct ITabStopManager;

class CDbComboBox : public XComboBox {
	std::shared_ptr<const IDbResultSet> result_set;
	size_t field_to_display, sel_index, empty_value_added;
	const size_t prim_key;
	ITabStopManager *manager;

	void fillComboBox();
	void OnItemChoosed(XCommandEvent *eve);
	void OnKeyPress(XKeyboardEvent *eve);

public:
	CDbComboBox(std::shared_ptr<const IDbResultSet> result_set, \
				const size_t field_to_display_, const size_t prim_key_);

	CDbComboBox(const CDbComboBox &obj) = delete;
	CDbComboBox(CDbComboBox &&obj) = default;
	CDbComboBox &operator=(const CDbComboBox &obj) = delete;
	CDbComboBox &operator=(CDbComboBox &&obj) = default;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	void setTabStopManager(ITabStopManager *manager_);
	inline std::shared_ptr<const IDbResultSet> getResultSet() const;
	int getPrimaryKeyAsInteger() const;
	inline bool isEmpty() const;
	inline void addEmptyValue();
	void SetCurrRecord(std::shared_ptr<const IDbResultSet> rs, \
						const size_t prim_key_no_in_rs, int prim_key_type_hint);

	virtual ~CDbComboBox();
};

//**************************************************************

std::shared_ptr<const IDbResultSet> CDbComboBox::getResultSet() const {

	return result_set;
}

bool CDbComboBox::isEmpty() const { 
	
	return sel_index == (size_t)-1 || (empty_value_added && !sel_index);
}

void CDbComboBox::addEmptyValue() {

	XComboBox::AddItem(_T("(0)"));
	empty_value_added = 1;
}
