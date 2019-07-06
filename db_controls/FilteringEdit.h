#pragma once
#include <xwindows/XEdit.h>

class CFilteringManager;

class CFilteringEdit : public XEdit {
	bool is_changed;
	int id_expr;
	CFilteringManager &filtering_manager;

	void OnChange(XCommandEvent *eve);
	void OnLooseFocus(XCommandEvent *eve);
public:
	CFilteringEdit(CFilteringManager &filtering_manager_);

	CFilteringEdit(const CFilteringEdit &obj) = delete;
	CFilteringEdit(CFilteringEdit &&obj) = default;
	CFilteringEdit &operator=(CFilteringEdit &&obj) = default;
	CFilteringEdit &operator=(const CFilteringEdit &obj) = delete;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	inline bool isChanged() const {	return is_changed; }
	inline void setExprId(const int id_expr) { this->id_expr = id_expr; }
	inline int getExprId() const { return id_expr; }

	virtual ~CFilteringEdit();
};

