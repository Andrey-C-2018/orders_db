#pragma once
#include <xwindows/XComboBox.h>
#include <xwindows_ex/XTabStopWidget.h>

class CFilteringManager;

class CFilteringComboBox : public XTabStopWidget<XComboBox> {
	size_t sel_index;
	CFilteringManager &filtering_manager;
	int id_expr;

	void OnItemChoosed(XCommandEvent *eve);

protected:
	inline void SetIdExpr(const int id_expr);
	virtual void fill() { }

public:
	CFilteringComboBox(CFilteringManager &filtering_manager_);

	CFilteringComboBox(const CFilteringComboBox &obj) = delete;
	CFilteringComboBox(CFilteringComboBox &&obj) = default;
	CFilteringComboBox &operator=(const CFilteringComboBox &obj) = delete;
	CFilteringComboBox &operator=(CFilteringComboBox &&obj) = default;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;
	inline bool isEmptySelection() const;

	virtual ~CFilteringComboBox();
};

//**************************************************************

bool CFilteringComboBox::isEmptySelection() const {

	return sel_index == (size_t)-1 || sel_index == 0;
}

void CFilteringComboBox::SetIdExpr(const int id_expr) {

	assert(this->id_expr == -1);
	this->id_expr = id_expr;
}