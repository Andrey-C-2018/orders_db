#pragma once
#include <memory>
#include <xwindows/XWidget.h>
#include "IReloadable.h"
#include "CellConfigurator.h"
#include "GridTableProxy.h"

class CGridException : public XException {
public:
	CGridException(const int err_code, const Tchar *err_descr);
	CGridException(const CGridException &obj);
	CGridException(CGridException &&obj) = default;
	~CGridException();
};

class CGrid : public XWidget, public IReloadable {
	std::shared_ptr<ITable> data_table;
	std::shared_ptr<CGridTableProxy> data_table_proxy;
	IGridCellsContainer *headers;
	IGridCellsContainer *cells;
	int width, height;
	int left_pane_width;
	int headers_height;

	XScrollParams hscroll, vscroll;

	XFont cells_font, headers_font;
	XColor cells_color, headers_color;
	XBrush grid_lines_brush, background_brush;
	XPen grid_lines_pen, background_pen;
	CCellConfigurator configurator;
	
	static bool is_class_registered;
	
	enum Defaults {
		DEF_LEFT_PANE_WIDTH = 30, \
		DEF_HEADERS_HEIGHT = 30
	};

	void OnCreate(XEvent *eve);
	void OnEraseBackground(XEvent *eve);
	void OnSize(XSizeEvent *eve);
	void OnPaint(XEvent *eve);	
	void OnHScroll(XScrollEvent *eve);
	void OnVScroll(XScrollEventEx *eve);
	void OnMouseLButtonClick(XMouseEvent *eve);

	inline int GetRecordsSizesSumm() const;
	inline void ReevaluateHScrollMax();
	inline void EvaluateScrollPos(const XScrollEvent *eve, \
									XScrollParams &scroll_params) const;

	CGrid(const CGrid &obj) = delete;
	CGrid &operator=(const CGrid &obj) = delete;

	void InitCellsAndHeaders(const int kind_of_layout);

protected:
	
	CGrid();
	void Init(std::shared_ptr<ITable> table, const int kind_of_layout);
	template <class TCell> \
		inline LayoutObjects CreateLayoutObjectsHelper(const int kind_of_layout);
	virtual LayoutObjects CreateLayoutObjects(const int kind_of_layout);

	virtual void OnWindowCreate() { }
	virtual void DrawLeftPane(XGC &gc) const;

	inline size_t GetVisibleRecordsCount() const;
	inline void FocusOnRecord(const size_t record_index);

public:
	void Create(XWindow *parent, const int flags,
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	inline size_t GetFieldsCount() const;
	inline size_t GetRecordsCount() const;

	inline void SetFieldWidth(const size_t field, const int new_width);
	void HideField(const size_t field_index);

	void Reload() override;
	virtual ~CGrid();
};

//**************************************************************

inline size_t CGrid::GetFieldsCount() const {

	return data_table_proxy->GetFieldsCount();
}

inline size_t CGrid::GetRecordsCount() const {

	return data_table_proxy->GetRecordsCount();
}

template <class TCell> \
inline LayoutObjects CGrid::CreateLayoutObjectsHelper(const int kind_of_layout) {

	return data_table_proxy->CreateLayoutObjects<TCell>(kind_of_layout);
}

void CGrid::SetFieldWidth(const size_t field, const int new_width) {

	data_table_proxy->SetFieldWidth(field, new_width);
}

int CGrid::GetRecordsSizesSumm() const {
	size_t records_count = data_table_proxy->GetRecordsCount();

	return (int)(records_count * configurator.GetCellHeight());
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

	return cells->GetVisibleRecordsCount(height);
}

void CGrid::FocusOnRecord(const size_t record_index) {

	vscroll.pos = cells->FocusOnRecord(record_index, height - headers_height);
	this->SetScrollBar(X_SCROLL_VERT, vscroll);
	Invalidate(nullptr, false);
}
