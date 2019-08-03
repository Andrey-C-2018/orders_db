#include "MultipleCellWidget.h"

CMultipleCellWidget::CMultipleCellWidget() : widgets_count(0), curr_widget(nullptr), \
												default_widget(nullptr), \
												default_widget_id(NOT_CREATED), \
												curr_field(0), parent(nullptr), flags(0) {

	widgets.reserve(DEF_WIDGETS_COUNT);
}

void CMultipleCellWidget::CreateCellWidget(XWindow *parent, const int flags, \
											const Tchar *label, \
											const int x, const int y, \
											const int width, const int height) {

	assert(default_widget);
	assert(default_widget_id == NOT_CREATED);

	default_widget->CreateCellWidget(parent, flags, label, x, y, width, height);
	this->parent = parent;
	this->default_widget_id = default_widget->GetId();
	this->flags = flags;
	SetEvtHandlers(default_widget);

	for (auto &widget : widgets) {
		assert(widget.id == NOT_CREATED);

		int widget_flags = flags;
		if (curr_widget != widget.widget)
			widget_flags &= (~FL_WINDOW_VISIBLE);

		widget.widget->CreateCellWidget(parent, widget_flags, label, x, y, width, height);
		widget.id = widget.widget->GetId();

		SetEvtHandlers(widget.widget);
	}
}

void CMultipleCellWidget::SetOnChangeHandler(XEventHandlerData on_change) {

	assert(this->on_change_handler.empty());
	this->on_change_handler = on_change;
	
	if (default_widget)
		default_widget->SetOnChangeHandler(std::move(on_change));

	for (auto &widget : widgets)
		if(widget.widget)
			widget.widget->SetOnChangeHandler(this->on_change_handler);
}

void CMultipleCellWidget::SetOnIndirectChangeHandler(std::shared_ptr<ICellEventHandler> handler) {

	assert(!this->on_indirect_change_handler);
	this->on_indirect_change_handler = handler;

	if (default_widget)
		default_widget->SetOnIndirectChangeHandler(handler);

	for (auto &widget : widgets)
		if (widget.widget)
			widget.widget->SetOnIndirectChangeHandler(handler);
}

void CMultipleCellWidget::SetOnGetFocusHandler(XEventHandlerData on_get_focus) {

	assert(this->on_get_focus_handler.empty());
	this->on_get_focus_handler = on_get_focus;

	if (default_widget)
		default_widget->SetOnGetFocusHandler(std::move(on_get_focus));

	for (auto &widget : widgets)
		if (widget.widget)
			widget.widget->SetOnGetFocusHandler(this->on_get_focus_handler);
}

void CMultipleCellWidget::SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) {

	assert(this->on_loose_focus_handler.empty());
	this->on_loose_focus_handler = on_loose_focus;

	if (default_widget)
		default_widget->SetOnLooseFocusHandler(std::move(on_loose_focus));

	for (auto &widget : widgets)
		if (widget.widget)
			widget.widget->SetOnLooseFocusHandler(this->on_loose_focus_handler);
}

void CMultipleCellWidget::SetOnKeyPressHandler(XEventHandlerData on_key_press) {

	assert(this->on_key_press_handler.empty());
	this->on_key_press_handler = on_key_press;

	if (default_widget)
		default_widget->SetOnKeyPressHandler(std::move(on_key_press));

	for (auto &widget : widgets)
		if (widget.widget)
			widget.widget->SetOnKeyPressHandler(this->on_key_press_handler);
}

void CMultipleCellWidget::SetDefaultWidget(IGridCellWidget *widget) {

	assert(widget);
	assert(widget != this);
	assert(default_widget_id == NOT_CREATED);
	
	if (default_widget)
		delete default_widget;
	
	default_widget = widget;

	if (!curr_widget) curr_widget = default_widget;
}

void CMultipleCellWidget::AddCellWidget(const size_t field, IGridCellWidget *widget) {

	assert(widget);
	assert(widget != this);
	assert(default_widget_id == NOT_CREATED);

	CCellWidget widget_item;
	widget_item.widget = widget;
	
	auto p_widget = std::find(widgets.begin(), widgets.end(), widget_item);
	if (p_widget == widgets.end()) {

		widget_item.id = NOT_CREATED;
		widget_item.ref_counter = 0;

		widgets.push_back(widget_item);
		++widgets_count;

		p_widget = widgets.begin() + widgets.size() - 1;
	}

	size_t widget_index = std::distance(widgets.begin(), p_widget);
	auto p = assignments.lower_bound(field);
	if (p != assignments.cend() && !assignments.key_comp()(field, p->first)) {
		
		++p_widget->ref_counter;
		--widgets[p->second].ref_counter;

		if (widgets[p->second].ref_counter == 0) {

			if(widgets[p->second].id == NOT_CREATED)
				delete widgets[p->second].widget;

			widgets[p->second].widget = nullptr;
			widgets[p->second].id = NOT_CREATED;
			--widgets_count;
		}

		p->second = widget_index;
	}
	else {
		assignments.insert(p, std::pair<size_t, size_t>(field, widget_index));
		++p_widget->ref_counter;
	}
		
	if (field == curr_field) curr_widget = widget;
}

void CMultipleCellWidget::SetCurrentField(const size_t field) {

	if (field == curr_field) return;
	curr_field = field;

	IGridCellWidget *prev_widget = curr_widget;

	auto p = assignments.lower_bound(field);
	if (p != assignments.cend() && !assignments.key_comp()(field, p->first))
		curr_widget = widgets[p->second].widget;
	else 
		curr_widget = default_widget;

	if (curr_widget != prev_widget) {
		prev_widget->Hide();
		curr_widget->Show();
		if (prev_widget->HasFocus()) curr_widget->SetFocus();
	}
}

void CMultipleCellWidget::Show() {

	assert(curr_widget);
	curr_widget->Show();
}

void CMultipleCellWidget::Hide() {

	assert(curr_widget);
	curr_widget->Hide();
}

void CMultipleCellWidget::MoveWindow(const int x, const int y, \
										const int width, const int height) {

	assert(curr_widget);
	curr_widget->MoveWindow(x, y, width, height);
}

void CMultipleCellWidget::SetFocus() {

	assert(curr_widget);
	curr_widget->SetFocus();
}

bool CMultipleCellWidget::HasFocus() const {

	assert(curr_widget);
	return curr_widget->HasFocus();
}

ImmutableString<Tchar> CMultipleCellWidget::GetLabel() {

	assert(curr_widget);
	return curr_widget->GetLabel();
}

void CMultipleCellWidget::SetLabel(ImmutableString<Tchar> label) {

	assert(curr_widget);
	curr_widget->SetLabel(label);
}

int CMultipleCellWidget::GetId() const {
	
	assert(curr_widget);
	return curr_widget->GetId();
}

bool CMultipleCellWidget::Validate() const {

	assert(curr_widget);
	return curr_widget->Validate();
}

bool CMultipleCellWidget::Validate(ImmutableString<Tchar> label) const {

	assert(curr_widget);
	return curr_widget->Validate(label);
}

CMultipleCellWidget::~CMultipleCellWidget(){

	for (auto &widget : widgets)
		if(widget.widget && widget.id == NOT_CREATED)
			delete widget.widget;

	if (default_widget && default_widget_id == NOT_CREATED)
		delete default_widget;
}
