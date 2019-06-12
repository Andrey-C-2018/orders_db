#pragma once
#include <vector>
#include <grid/IDrawable.h>
#include <grid/TextCell.h>
#include "IGridCellWidget.h"
#include "IGridEventsHandler.h"

class CGridTableProxy;

class CDispatcherCell : public IDrawable {
	IGridCellWidget *def_active_cell;
	CTextCell unactive_cell;
	size_t active_field, active_record;
	bool active_cell_reached;

	ImmutableString<Tchar> active_cell_text;
	XColor active_cell_color;

	XPoint curr_coords, def_act_cell_coords;
	XSize curr_size;

	bool active_cell_hidden;
	bool move_def_cell, inside_on_click;
	int old_scroll_pos;

	XRect bounds;
	std::shared_ptr<CGridTableProxy> table_proxy;
	std::shared_ptr<IGridEventsHandler> event_handler;

	bool update_cell_widget_text;
	bool changes_present;
	bool skip_reloading;

	void CommitChangesIfPresent();
	void OnTableChanged(const size_t old_field, const size_t old_record);
	void RefreshActiveCellWidgetLabel();
public:
	CDispatcherCell();
	CDispatcherCell(const CDispatcherCell &obj) = delete;
	CDispatcherCell(CDispatcherCell &&obj);

	CDispatcherCell &operator=(const CDispatcherCell &obj) = delete;
	CDispatcherCell &operator=(CDispatcherCell &&obj) = delete;

	inline size_t GetActiveField() const;
	inline size_t GetActiveRecord() const;

	void SetBounds(const int left_bound, const int upper_bound) override;
	void SetParameters(XWindow * parent, \
						std::shared_ptr<IGridEventsHandler> grid_events_handler, \
						IGridCellWidget *cell_widget, \
						std::shared_ptr<CGridTableProxy> table_proxy);

	inline void BeginScroll(const int scroll_pos);
	inline void EndScroll(const int scroll_pos);

	inline void Init(const ImmutableString<Tchar> str, \
					const size_t field, const size_t record);
	void Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) override;

	void OnActiveCellTextChanged(XCommandEvent *eve);
	void OnClick(const size_t field, const size_t record);
	void OnActiveCellLoosesFocus(XEvent *eve);
	void OnActiveCellKeyPressed(XKeyboardEvent *eve);

	inline void OnFieldRemoved(const size_t field);

	inline int EvalCellHeightByTextHeight(const int text_height) const;
	inline int GetMarginsWidth() const;

	inline void SetFocus();
	void Reload();

	virtual ~CDispatcherCell(){ }
};

//**************************************************************

int CDispatcherCell::EvalCellHeightByTextHeight(const int text_height) const {

	return unactive_cell.EvalCellHeightByTextHeight(text_height);
}

int CDispatcherCell::GetMarginsWidth() const {

	return unactive_cell.GetMarginsWidth();
}

size_t CDispatcherCell::GetActiveField() const {

	return this->active_field;
}

size_t CDispatcherCell::GetActiveRecord() const {

	return this->active_record;
}

void CDispatcherCell::BeginScroll(const int scroll_pos) {

	if (scroll_pos == old_scroll_pos) return;
	old_scroll_pos = scroll_pos;

	if (!active_cell_hidden) {
		def_active_cell->Hide();
		active_cell_hidden = true;
	}
	move_def_cell = false;
}

void CDispatcherCell::EndScroll(const int scroll_pos) {

	move_def_cell = true;
}

void CDispatcherCell::Init(const ImmutableString<Tchar> str, \
							const size_t field, const size_t record) {

	active_cell_reached = (field == active_field && record == active_record);
	if (!active_cell_reached || \
		(active_cell_reached && active_cell_hidden && !changes_present))
		unactive_cell.Init(str, field, record);

	if (active_cell_reached && active_cell_hidden && changes_present) {

		active_cell_text = def_active_cell->GetLabel();
		unactive_cell.Init(active_cell_text, field, record);
	}
}

void CDispatcherCell::SetFocus() {

	assert(def_active_cell);
	def_active_cell->SetFocus();
}

void CDispatcherCell::OnFieldRemoved(const size_t field) {

	if (field <= active_field) {
		
		CommitChangesIfPresent();

		size_t prev_active_field = active_field;
		if (field == active_field)
			active_field = active_field ? \
							active_field - 1 : 1;
		else
			--active_field;

		def_active_cell->SetCurrentField(active_field);

		OnTableChanged(prev_active_field, active_record);
	}
}