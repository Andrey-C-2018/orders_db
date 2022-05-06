#pragma once
#include <memory>
#include <xwindows/XWidget.h>
#include "IReloadable.h"
#include "IGCParamsList.h"
#include "CellConfigurator.h"
#include "GridTableProxy.h"

class CGridException : public XException {
public:
	enum {
		E_WRONG_FIELD = 1, \
		E_HIDE_FIELD = 2
	};
	CGridException(const int err_code, const Tchar *err_descr);
	CGridException(const CGridException &obj);
	CGridException(CGridException &&obj) = default;
	virtual ~CGridException();
};

class CGrid : public XWidget, public IReloadable {
protected:
	std::shared_ptr<CGridTableProxy> data_table_proxy;

private:
	std::shared_ptr<ITable> data_table;

	IGridCellsContainer *headers;
	IGridCellsContainer *cells;
	int width, height;
	int left_pane_width;
	int headers_height;

	int single_char_width;

	XScrollParams hscroll, vscroll;
	std::shared_ptr<CCellConfigurator> headers_configurator, cells_configurator;
	std::shared_ptr<IGCParamsList> gc_global_params, gc_headers_params;
	
	static bool is_class_registered;
	
	enum Defaults {
		DEF_LEFT_PANE_WIDTH = 20, \
		DEF_HEADERS_HEIGHT = 30, \
		WHEEL_STEP = 20
	};

	void OnCreate(XEvent *eve);
	void OnEraseBackground(XEvent *eve);
	void OnSize(XSizeEvent *eve);
	void OnPaint(XEvent *eve);	
	void OnHScroll(XScrollEvent *eve);
	void OnVScroll(XScrollEventEx *eve);
	void OnMouseLButtonClick(XMouseEvent *eve);
	void OnMouseWheel(XMouseWheelEvent *eve);

	inline int GetRecordsSizesSumm() const;
	inline void ReevaluateHScrollMax();
	inline void EvaluateScrollPos(const XScrollEvent *eve, \
									XScrollParams &scroll_params) const;

	void InitCellsAndHeaders(const int kind_of_layout);
	inline void CheckWhetherAbsFieldIndexValid(const size_t abs_field_index) const;
	inline void CheckWhetherFieldIndexValid(const size_t field_index) const;

	CGrid(const CGrid &obj) = delete;
	CGrid &operator=(const CGrid &obj) = delete;
protected:
	
	CGrid();
	void Init(std::shared_ptr<ITable> table, const int kind_of_layout);

	virtual GridConfigurators CreateConfigurators() = 0;
	virtual GridGCParamsLists CreateGCParamsLists() = 0;

	template <class TCell> \
		inline LayoutObjects CreateLayoutObjectsHelper(const int kind_of_layout);
	virtual LayoutObjects CreateLayoutObjects(const int kind_of_layout);

	virtual void OnWindowCreate() { }
	virtual void DrawLeftPane(XGC &gc) const;

	inline size_t GetVisibleRecordsCount() const;
	inline int GetLeftPaneSize() const;
	inline int GetHeight() const;
	inline int GetHeadersHeight() const;
	inline int GetRecordHeight() const;
	inline int GetVScrollPos() const;

	inline void FocusOnRecord(const size_t record_index);
	inline void FocusOnField(const size_t field_index);

public:
	void Create(XWindow *parent, const int flags,
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	CGrid(CGrid &&obj) = default;
	CGrid &operator=(CGrid &&obj) = default;

	inline size_t GetFieldsCount() const;
	inline size_t GetRecordsCount() const;

	inline void SetFieldWidth(const size_t field, const int new_width);
	inline const Tchar *GetFieldLabelAbs(const size_t field) const;
	inline void SetFieldLabel(const size_t field, const Tchar *label);
	void HideField(const size_t field_index);
	inline size_t GetFieldRelativeIndex(const size_t abs_field_index) const;

	void Reload() override;
	virtual ~CGrid();
};

//**************************************************************

size_t CGrid::GetFieldsCount() const {

	return data_table_proxy->GetFieldsCount();
}

size_t CGrid::GetRecordsCount() const {

	return data_table_proxy->GetRecordsCount();
}

template <class TCell> \
LayoutObjects CGrid::CreateLayoutObjectsHelper(const int kind_of_layout) {

	return data_table_proxy->CreateLayoutObjects<TCell>(kind_of_layout);
}

size_t CGrid::GetFieldRelativeIndex(const size_t abs_field_index) const {

	CheckWhetherAbsFieldIndexValid(abs_field_index);
	return data_table_proxy->GetRelativeFieldIndex(abs_field_index);
}

void CGrid::SetFieldWidth(const size_t field, const int new_width) {

	CheckWhetherFieldIndexValid(field);
	assert(!IsCreated());
	data_table_proxy->SetFieldWidth(field, new_width);
}

const Tchar *CGrid::GetFieldLabelAbs(const size_t field) const {

	CheckWhetherAbsFieldIndexValid(field);
	return data_table_proxy->GetFieldNameAbs(field);
}

void CGrid::SetFieldLabel(const size_t field, const Tchar *label) {

	CheckWhetherFieldIndexValid(field);
	assert(!IsCreated());
	data_table_proxy->SetFieldName(field, label);
}

int CGrid::GetRecordsSizesSumm() const {
	size_t records_count = data_table_proxy->GetRecordsCount();

	return (int)(records_count * cells_configurator->GetCellHeight());
}

void CGrid::ReevaluateHScrollMax() {
	auto fields_props = data_table_proxy->GetFieldsProperties();

	hscroll.max = fields_props->GetFieldsSizesSumm();
	this->SetScrollBar(X_SCROLL_HORZ, hscroll);
}

void CGrid::EvaluateScrollPos(const XScrollEvent *eve, \
								XScrollParams &scroll_params) const {

	auto LimitToLowerBound = [](const int value, const int lower_bound) {
		return (value >= lower_bound) ? value : lower_bound;
	};
	auto LimitToUpperBound = [](const int value, const int upper_bound) {
		return (value <= upper_bound) ? value : upper_bound;
	};

	switch (eve->GetScrollType()) {
	case X_SCROLL_THUMBTRACK:
		scroll_params.pos = eve->GetThumbPos();
		break;

	case X_SCROLL_LINEUP:
		scroll_params.pos = LimitToLowerBound(scroll_params.pos - 1, 0);
		break;

	case X_SCROLL_LINEDOWN:
		scroll_params.pos = LimitToUpperBound(scroll_params.pos + 1, \
			scroll_params.max - scroll_params.page + 1);
		break;

	case X_SCROLL_PAGEUP:
		scroll_params.pos = LimitToLowerBound(scroll_params.pos - \
			scroll_params.page, 0);
		break;

	case X_SCROLL_PAGEDOWN:
		scroll_params.pos = LimitToUpperBound(scroll_params.pos + \
			scroll_params.page, \
			scroll_params.max - scroll_params.page + 1);
	}
}

size_t CGrid::GetVisibleRecordsCount() const {

	return cells->GetVisibleRecordsCount(height - headers_height);
}

int CGrid::GetLeftPaneSize() const {

	return left_pane_width;
}

int CGrid::GetHeight() const {

	return height;
}

int CGrid::GetHeadersHeight() const {

	return headers_height;
}

int CGrid::GetRecordHeight() const {

	return cells_configurator->GetCellHeight();
}

int CGrid::GetVScrollPos() const {

	return vscroll.pos;
}

void CGrid::FocusOnRecord(const size_t record_index) {

	vscroll.pos = cells->FocusOnRecord(record_index, height - headers_height);
	this->SetScrollBar(X_SCROLL_VERT, vscroll);
}

void CGrid::FocusOnField(const size_t field_index) {

	hscroll.pos = cells->FocusOnField(field_index, hscroll.page, hscroll.max);
	this->SetScrollBar(X_SCROLL_HORZ, hscroll);
}

void CGrid::CheckWhetherAbsFieldIndexValid(const size_t abs_field_index) const {

	if (abs_field_index >= data_table_proxy->GetAbsoluteFieldsCount()) {

		CGridException e(CGridException::E_WRONG_FIELD, \
						_T("No such field: absolute field index = "));
		e << abs_field_index;
		throw e;
	}
}

void CGrid::CheckWhetherFieldIndexValid(const size_t field_index) const {

	if (field_index >= data_table_proxy->GetFieldsCount()) {

		CGridException e(CGridException::E_WRONG_FIELD, \
						_T("No such field: field index = "));
		e << field_index;
		throw e;
	}
}
