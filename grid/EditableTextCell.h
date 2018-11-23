#pragma once
#include <vector>
#include "IDrawable.h"
#include "ICellWidget.h"
#include "TextCell.h"
#include "xwindows/XEdit.h"

class CGridTableProxy;

class CEditableTextCell : public IDrawable {
	XEdit *def_active_cell;
	CTextCell unactive_cell;
	size_t active_field, active_record;
	bool active_cell_reached;

	const Tchar *active_cell_text;
	size_t active_cell_text_len;
	XColor active_cell_color;

	XPoint curr_coords;
	XSize curr_size;

	bool active_cell_hidden;
	bool scroll_ended;
	int old_scroll_pos;

	XRect bounds;
	std::shared_ptr<CGridTableProxy> table_proxy;

	bool update_cell_widget_text;
	bool changes_present;

	void CommitChangesIfPresent();
	void OnActiveCellLocationChanged();
public:
	CEditableTextCell();

	inline size_t GetActiveField() const;
	inline size_t GetActiveRecord() const;

	void SetBounds(const int left_bound, const int upper_bound) override;
	void SetParameters(XWindow *parent, std::shared_ptr<CGridTableProxy> table_procy);

	inline void BeginScroll(const int scroll_pos);
	inline void EndScroll(const int scroll_pos);

	inline void Init(const ImmutableString<Tchar> str, \
					const size_t field, const size_t record);
	void Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) override;

	void OnClick(const size_t field, const size_t record);
	void OnActiveCellTextChanged(XCommandEvent *eve);
	inline void OnFieldRemoved(const size_t field);

	inline int EvalCellHeightByTextHeight(const int text_height) const;

	inline void SetFocus();

	virtual ~CEditableTextCell(){ }
};

//**************************************************************

int CEditableTextCell::EvalCellHeightByTextHeight(const int text_height) const {

	return unactive_cell.EvalCellHeightByTextHeight(text_height);
}

size_t CEditableTextCell::GetActiveField() const {

	return this->active_field;
}

size_t CEditableTextCell::GetActiveRecord() const {

	return this->active_record;
}

void CEditableTextCell::BeginScroll(const int scroll_pos) {

	if (scroll_pos == old_scroll_pos) return;
	old_scroll_pos = scroll_pos;

	if (!active_cell_hidden) {
		def_active_cell->Hide();
		active_cell_hidden = true;
	}
	scroll_ended = false;
}

void CEditableTextCell::EndScroll(const int scroll_pos) {

	scroll_ended = true;
}

void CEditableTextCell::Init(const ImmutableString<Tchar> str, \
							const size_t field, const size_t record) {

	active_cell_reached = (field == active_field && record == active_record);
	if (!active_cell_reached || \
		(active_cell_reached && active_cell_hidden && !changes_present))
		unactive_cell.Init(str, field, record);

	if (active_cell_reached && active_cell_hidden && changes_present) {

		active_cell_text = def_active_cell->GetLabel(active_cell_text_len);
		ImmutableString<Tchar> preserved_str(active_cell_text, \
										active_cell_text_len);
		unactive_cell.Init(preserved_str, field, record);
	}
}

void CEditableTextCell::SetFocus() {

	assert(def_active_cell);
	def_active_cell->SetFocus();
}

void CEditableTextCell::OnFieldRemoved(const size_t field) {

	if (field <= active_field) {
		
		CommitChangesIfPresent();
		if (field == active_field)
			active_field = active_field ? \
							active_field - 1 : 1;
		else
			--active_field;

		OnActiveCellLocationChanged();
	}
}