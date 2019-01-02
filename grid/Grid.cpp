#include "TextCell.h"
#include "Grid.h"

CGridException::CGridException(const int err_code, const Tchar *err_descr) : \
XException(err_code, err_descr) { }

CGridException::CGridException(const CGridException &obj) : XException(obj) { }

CGridException::~CGridException() { }

//****************************************************************************

class CHeadersData final {
	std::shared_ptr<const IFieldsProperties> fields_props;
public:
	CHeadersData(std::shared_ptr<const IFieldsProperties> fields_props_) : \
					fields_props(fields_props_) { }

	CHeadersData(const CHeadersData &obj) = default;
	CHeadersData(CHeadersData &&obj) = default;
	CHeadersData &operator=(const CHeadersData &obj) = default;
	CHeadersData &operator=(CHeadersData &&obj) = default;

	inline ImmutableString<Tchar> operator[](const size_t index) {

		return fields_props->GetFieldName(index);
	}

	inline std::pair<size_t, size_t> GetCellCoords(const size_t index) {

		return(std::pair<size_t, size_t>(index, 0));
	}

	inline size_t size() const { 

		return fields_props->size();
	}
};

//**************************************************************

bool CGrid::is_class_registered = false;

CGrid::CGrid() : headers(nullptr), cells(nullptr), \
				width(0), height(0), \
				left_pane_width(DEF_LEFT_PANE_WIDTH), \
				headers_height(DEF_HEADERS_HEIGHT), \
				cells_font(20, 0, 0, 0, RUSSIAN_CHARSET, _T("Consolas")), \
				headers_font(25, 0, 0, 0, RUSSIAN_CHARSET, _T("Consolas")), \
				cells_color(170, 170, 170), headers_color(128, 128, 128), \
				grid_lines_brush(0, 0, 200), background_brush(100, 100, 100), \
				grid_lines_pen(XPEN_SOLID, 1, 0, 0, 200), \
				background_pen(XPEN_SOLID, 1, 100, 100, 100){

	if (!is_class_registered) {
		RegisterNewWidgetClass(_T("XGRIDCONTROL"));
		is_class_registered = true;
	}
	else CheckAndInitWindowClassName(_T("XGRIDCONTROL"));
}

void CGrid::Init(std::shared_ptr<ITable> table, const int kind_of_layout) {

	assert(cells == nullptr);
	assert(headers == nullptr);

	data_table = table;
	data_table_proxy = std::make_shared<CGridTableProxy>(data_table, this);
	data_table->ConnectEventsHandler(data_table_proxy);

	InitCellsAndHeaders(kind_of_layout);
}

LayoutObjects CGrid::CreateLayoutObjects(const int kind_of_layout) {

	return data_table_proxy->CreateLayoutObjects<CTextCell>(kind_of_layout);
}

void CGrid::InitCellsAndHeaders(const int kind_of_layout) {

	LayoutObjects l = CreateLayoutObjects(kind_of_layout);
	cells = l.first;
	ISharedComposerContainer *shared_composer_container = l.second;
	assert(cells);
	assert(shared_composer_container);

	typedef TSimpleRecordLayout<CHeadersData, CTextCell> CHeadersRecord;
	headers = new CHeadersRecord(CHeadersData(data_table_proxy->GetFieldsProperties()), \
									shared_composer_container->getComposer());
}

void CGrid::Create(XWindow *parent, const int flags, \
	const Tchar *label, \
	const int x, const int y, \
	const int width, const int height) {

	this->width = width;
	this->height = height;

	Connect(EVT_CREATE, this, &CGrid::OnCreate);
	XWidget::Create(parent, flags | FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED, \
					_T(""), x, y, width, height);
}

void CGrid::OnCreate(XEvent *eve) {
	XStandAloneGC gc(this);
	
	assert(headers);
	assert(cells);
	assert(height > 0);

	configurator.Init(&gc, &headers_font, \
						&background_brush, &background_pen);

	headers->AcceptConfigurator(&configurator);
	headers_height = configurator.GetCellHeight();
	headers->SetItemHeight(headers_height);
	headers->SetBounds(left_pane_width, 0);

	configurator.Init(&gc, &cells_font, \
						&background_brush, &background_pen);
		
	cells->AcceptConfigurator(&configurator);
	cells->SetItemHeight(configurator.GetCellHeight());
	cells->SetBounds(left_pane_width, headers_height);

	int single_char_width = configurator.GetCharWidthInPixels();
	int cell_margins_width = configurator.GetCellMarginsWidth();

	auto fields_props = data_table_proxy->GetFieldsProperties();
	fields_props->TransformAllSizes(cell_margins_width, single_char_width);

	hscroll.max = fields_props->GetFieldsSizesSumm() - 1;
	hscroll.page = width - left_pane_width;
	hscroll.pos = 0;
	this->SetScrollBar(X_SCROLL_HORZ, hscroll);

	vscroll.max = GetRecordsSizesSumm() - 1;
	vscroll.page = height - headers_height;
	vscroll.pos = 0;
	this->SetScrollBar(X_SCROLL_VERT, vscroll);

	Connect(EVT_ERASEBKGND, this, &CGrid::OnEraseBackground);
	Connect(EVT_SIZE, this, &CGrid::OnSize);
	Connect(EVT_PAINT, this, &CGrid::OnPaint);
	Connect(EVT_HSCROLL, this, &CGrid::OnHScroll);
	Connect(EVT_VSCROLL, this, &CGrid::OnVScroll);
	Connect(EVT_LBUTTONDOWN, this, &CGrid::OnMouseLButtonClick);
	Connect(EVT_MOUSEWHEEL, this, &CGrid::OnMouseWheel);

	OnWindowCreate();
}

void CGrid::OnEraseBackground(XEvent *eve) { }

void CGrid::OnSize(XSizeEvent *eve) {

	this->width = eve->GetWidth();
	this->height = eve->GetHeight();
	if (width < left_pane_width || \
		height < headers_height) return;

	hscroll.page = width - left_pane_width;
	this->SetScrollBar(X_SCROLL_HORZ, hscroll);
	vscroll.page = height - headers_height;
	this->SetScrollBar(X_SCROLL_VERT, vscroll);

	hscroll.pos = GetScrollPos(X_SCROLL_HORZ);
	vscroll.pos = GetScrollPos(X_SCROLL_VERT);

	assert(cells);
	cells->BeginHScroll(hscroll.pos);
	cells->BeginVScroll(vscroll.pos);

	Invalidate(nullptr, false);
}

void CGrid::DrawLeftPane(XGC &gc) const {

	gc.Rectangle(0, 0, left_pane_width, height);
}

void CGrid::OnPaint(XEvent *eve) {
	XPaintEventGC gc(this);
	int old_bk_mode = gc.SetBackgroundFillMode(X_BK_TRANSPARENT);
	XColor old_bg_color = gc.SetBackgroundColor(cells_color);
	XFont old_font = gc.SelectObject(cells_font);
	XPen old_pen = gc.SelectObject(grid_lines_pen);
	XBrush old_brush = gc.SelectObject(grid_lines_brush);
	
	DrawLeftPane(gc);

	gc.SetBounds(left_pane_width, headers_height);
	cells->Draw(gc, XPoint(left_pane_width, headers_height), \
					XSize(width - left_pane_width, height - headers_height));

	gc.SetBackgroundColor(headers_color);
	gc.SelectObject(headers_font);

	gc.SetBounds(left_pane_width, 0);
	headers->Draw(gc, XPoint(left_pane_width, 0), \
					XSize(width - left_pane_width, headers_height));

	gc.SelectObject(old_pen);
	gc.SelectObject(old_brush);
	gc.SelectObject(old_font);
	gc.SetBackgroundColor(old_bg_color);
	gc.SetBackgroundFillMode(old_bk_mode);
}

void CGrid::OnHScroll(XScrollEvent *eve) {

	EvaluateScrollPos(eve, hscroll);
	if (eve->GetScrollType() != X_SCROLL_END)
		cells->BeginHScroll(hscroll.pos);
	else
		cells->EndScroll(hscroll.pos);

	this->SetScrollBar(X_SCROLL_HORZ, hscroll);
	Invalidate(nullptr, false);
}

void CGrid::OnVScroll(XScrollEventEx *eve) {

	EvaluateScrollPos(eve, vscroll);
	if (eve->GetScrollType() != X_SCROLL_END)
		cells->BeginVScroll(vscroll.pos);
	else
		cells->EndScroll(vscroll.pos);

	this->SetScrollBar(X_SCROLL_VERT, vscroll);
	Invalidate(nullptr, false);
}

void CGrid::OnMouseLButtonClick(XMouseEvent *eve) {
	int x = eve->GetX();
	int y = eve->GetY();

	if (x < left_pane_width || y < headers_height)
		return;

	cells->OnClick(x, y);
	Invalidate(nullptr, false);
}

void CGrid::OnMouseWheel(XMouseWheelEvent *eve) {
	auto LimitToLowerBound = [](const int value, const int lower_bound) {
		return (value >= lower_bound) ? value : lower_bound;
	};
	auto LimitToUpperBound = [](const int value, const int upper_bound) {
		return (value <= upper_bound) ? value : upper_bound;
	};

	if (vscroll.page < vscroll.max) {
		if (eve->GetDelta() >= 0)
			vscroll.pos = LimitToLowerBound(vscroll.pos - WHEEL_STEP, 0);
		else
			vscroll.pos = LimitToUpperBound(vscroll.pos + WHEEL_STEP, \
											vscroll.max - vscroll.page + 1);

		cells->BeginVScroll(vscroll.pos);
		cells->EndScroll(vscroll.pos);

		this->SetScrollBar(X_SCROLL_VERT, vscroll);
		Invalidate(nullptr, false);
	}
}

void CGrid::HideField(const size_t field_index) {

	CheckWhetherAbsFieldIndexValid(field_index);

	if (data_table_proxy->GetFieldsCount() == 1)
		throw CGridException(CGridException::E_HIDE_FIELD, \
							_T("There is just one field in the table left visible"));

	if (data_table_proxy->IsFieldHidden(field_index)) return;

	const size_t relative_field_index = data_table_proxy->GetRelativeFieldIndex(field_index);

	hscroll.pos = cells->RemoveField(relative_field_index, \
										hscroll.page, hscroll.max);
	data_table_proxy->HideField(field_index);

	if (IsCreated()) {
		ReevaluateHScrollMax();
		Invalidate(nullptr, false);
	}
}

void CGrid::Reload() {

	if (IsCreated()) {
		auto fields_props = data_table_proxy->GetFieldsProperties();

		int summ = fields_props->GetFieldsSizesSumm();
		if (hscroll.max != summ) {
			
			hscroll.max = summ;
			this->SetScrollBar(X_SCROLL_HORZ, hscroll);
		}

		cells->Reload();
		Invalidate(nullptr, false);
	}

}

CGrid::~CGrid() {

	data_table->DisconnectEventsHandler(data_table_proxy);

	if (headers) delete headers;
	headers = nullptr;

	if (cells) delete cells;
	cells = nullptr;
}