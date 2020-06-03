#pragma once
#include <xwindows_ex/XDateField.h>

class CFilteringManager;

class CFilteringDateField : public XDateField {
	bool is_changed;
	int id_expr;
	CFilteringManager &filtering_manager;

	void OnChange() override;
	void OnLooseFocus(XCommandEvent *eve);

public:
	CFilteringDateField(CFilteringManager &filtering_manager_, ITabStopManager *manager);

	CFilteringDateField(const CFilteringDateField &obj) = delete;
	CFilteringDateField(CFilteringDateField &&obj) = default;
	CFilteringDateField &operator=(CFilteringDateField &&obj) = default;
	CFilteringDateField &operator=(const CFilteringDateField &obj) = delete;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	inline bool isChanged() const { return is_changed; }
	inline void setExprId(const int id_expr) { this->id_expr = id_expr; }
	inline int getExprId() const { return id_expr; }
	void enableIfChanged();

	virtual ~CFilteringDateField();
};

//*****************************************************