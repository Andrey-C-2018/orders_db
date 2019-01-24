#include "MultipleCellWidget.h"

CMultipleCellWidget::CMultipleCellWidget() : curr_widget(nullptr), default_widget(nullptr), \
												default_widget_id(NOT_CREATED), \
												curr_field(0), parent(nullptr), flags(0) { }

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

	for (auto &widget : widgets) {
		assert(widget.second.id == NOT_CREATED);

		int widget_flags = flags;
		if (curr_field != widget.first)
			widget_flags &= (~FL_WINDOW_VISIBLE);

		widget.second.widget->CreateCellWidget(parent, widget_flags, label, x, y, width, height);
		widget.second.id = widget.second.widget->GetId();

		if(!this->on_change_handler.empty())
			widget.second.widget->SetOnChangeHandler(this->on_change_handler);
		if (!this->on_loose_focus_handler.empty())
			widget.second.widget->SetOnLooseFocusHandler(this->on_loose_focus_handler);
		if (!this->on_key_press_handler.empty())
			widget.second.widget->SetOnKeyPressHandler(this->on_key_press_handler);
	}
}

void CMultipleCellWidget::SetOnChangeHandler(XEventHandlerData on_change) {

	assert(default_widget);
	this->on_change_handler = on_change;
	
	if (default_widget_id != NOT_CREATED) {
		default_widget->SetOnChangeHandler(std::move(on_change));

		for (auto &widget : widgets)
			widget.second.widget->SetOnChangeHandler(this->on_change_handler);
	}
}

void CMultipleCellWidget::SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) {

	/*assert(default_widget);
	this->on_loose_focus_handler = on_loose_focus;
	default_widget->SetOnLooseFocusHandler(std::move(on_loose_focus));*/
}

void CMultipleCellWidget::SetOnKeyPressHandler(XEventHandlerData on_key_press) {

	/*assert(default_widget);
	this->on_key_press_handler = on_key_press;
	default_widget->SetOnKeyPressHandler(std::move(on_key_press));*/
}

void CMultipleCellWidget::SetDefaultWidget(IGridCellWidget *widget) {

	assert(widget);
	assert(widget != this);
	if (default_widget && default_widget_id == NOT_CREATED)
		delete default_widget;

	default_widget = widget;
	if (default_widget_id != NOT_CREATED) {
		default_widget->CreateCellWidget(parent, flags, _T(""), 0, 0, 0, 0);
		default_widget_id = default_widget->GetId();
	}
}

void CMultipleCellWidget::AddCellWidget(const size_t field, IGridCellWidget *widget) {

	assert(widget);
	assert(widget != this);
	auto p = widgets.lower_bound(field);
	if (p != widgets.cend() && !widgets.key_comp()(field, p->first) && \
		p->second.id == NOT_CREATED)
		delete p->second.widget;
	else {
		CCellWidget widget_item;
		widget_item.id = NOT_CREATED;
		widget_item.widget = widget;
		p = widgets.insert(p, std::pair<size_t, CCellWidget>(field, widget_item));
	}

	if (default_widget_id != NOT_CREATED) {
		widget->CreateCellWidget(parent, flags, _T(""), 0, 0, 0, 0);
		p->second.id = widget->GetId();
	}
}

void CMultipleCellWidget::SetCurrentField(const size_t field) {

	if (field == curr_field) return;
	curr_field = field;

	IGridCellWidget *prev_widget = curr_widget;

	auto p = widgets.lower_bound(field);
	if (p != widgets.cend() && !widgets.key_comp()(field, p->first))
		curr_widget = p->second.widget;
	else 
		curr_widget = default_widget;

	if (curr_widget != prev_widget) {
		prev_widget->Hide();
		curr_widget->Show();
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

CMultipleCellWidget::~CMultipleCellWidget(){

	for (auto &widget : widgets)
		if(widget.second.id == NOT_CREATED)
			delete widget.second.widget;

	if (default_widget && default_widget_id == NOT_CREATED)
		delete default_widget;
}
