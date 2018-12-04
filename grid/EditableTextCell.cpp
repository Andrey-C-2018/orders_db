#include "EditableTextCell.h"
#include "GridTableProxy.h"

CEditableTextCell::CEditableTextCell() : def_active_cell(nullptr), \
									active_field(0), active_record(0), \
									active_cell_reached(false), \
									active_cell_text(nullptr), \
									active_cell_text_len(0), \
									active_cell_color(255, 255, 255), \
									active_cell_hidden(true), \
									scroll_ended(true), \
									old_scroll_pos(0), table_proxy(nullptr), \
									update_cell_widget_text(true), \
									changes_present(false), \
									skip_reloading(false) { }

CEditableTextCell::CEditableTextCell(CEditableTextCell &&obj) : \
									def_active_cell(obj.def_active_cell), \
									active_field(obj.active_field), \
									active_record(obj.active_record), \
									active_cell_reached(obj.active_cell_reached), \
									active_cell_text(obj.active_cell_text), \
									active_cell_text_len(obj.active_cell_text_len), \
									active_cell_color(255, 255, 255), \
									active_cell_hidden(obj.active_cell_hidden), \
									scroll_ended(obj.scroll_ended), \
									old_scroll_pos(obj.old_scroll_pos), \
									table_proxy(std::move(obj.table_proxy)), \
									update_cell_widget_text(obj.update_cell_widget_text), \
									changes_present(obj.changes_present), \
									skip_reloading(obj.skip_reloading) {

	obj.def_active_cell = nullptr;
	obj.active_field = 0;
	obj.active_record = 0;
	obj.active_cell_reached = false;
	obj.active_cell_text = nullptr;
	obj.active_cell_text_len = 0;
	obj.active_cell_hidden = true;
	obj.scroll_ended = true;
	obj.old_scroll_pos = 0;
	obj.update_cell_widget_text = true;
	obj.changes_present = false;
	obj.skip_reloading = false;
}

void CEditableTextCell::SetBounds(const int left_bound, const int upper_bound) {

	bounds.left = left_bound;
	bounds.top = upper_bound;
}

void CEditableTextCell::SetParameters(XWindow *parent, std::shared_ptr<CGridTableProxy> table_proxy) {

	int flags = FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL;

	assert(!def_active_cell);
	assert(parent);

	assert(!this->table_proxy);
	assert(table_proxy);
	this->table_proxy = table_proxy;

	ImmutableString<Tchar> label(nullptr, 0);
	if (table_proxy->GetRecordsCount())
		label = table_proxy->GetCellAsString(active_field, active_record);

	def_active_cell = new XEdit(parent, flags, label.str ? label.str : _T(""), \
									0, 0, 10, 10);
	assert(def_active_cell);

	parent->Connect(EVT_COMMAND, NCODE_EDIT_CHANGED, def_active_cell->GetId(), \
						this, &CEditableTextCell::OnActiveCellTextChanged);
}

void CEditableTextCell::Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) {

	if (active_cell_reached) {
		if (initial_coords != curr_coords || size != curr_size || \
			(active_cell_hidden && scroll_ended)) {

			int x(initial_coords.x), y(initial_coords.y);
			int width(size.width), height(size.height);

			if (initial_coords.x < bounds.left) {
				x = bounds.left;
				width = size.width - (bounds.left - initial_coords.x);
			}
			if (initial_coords.y < bounds.top) {
				y = bounds.top;
				height = size.height - (bounds.top - initial_coords.y);
			}

			assert(def_active_cell);
			if (!active_cell_hidden || (active_cell_hidden && scroll_ended))
				def_active_cell->MoveWindow(x, y, width, height);

			if (active_cell_hidden && scroll_ended) {
				def_active_cell->Show();
				active_cell_hidden = false;
			}

			curr_coords = initial_coords;
			curr_size = size;
		}
		active_cell_reached = false;

		if (active_cell_hidden) {
			XColor old_bg_color = gc.SetBackgroundColor(active_cell_color);
			unactive_cell.Draw(gc, initial_coords, size);
			gc.SetBackgroundColor(old_bg_color);
		}
	}
	else
		unactive_cell.Draw(gc, initial_coords, size);
}

void CEditableTextCell::OnActiveCellTextChanged(XCommandEvent *eve) {

	if (update_cell_widget_text) return;
	
	changes_present = true;
}

void CEditableTextCell::OnClick(const size_t field, const size_t record) {

	CommitChangesIfPresent();
	size_t new_records_count = table_proxy->GetRecordsCount();

	active_field = field;

	if (!new_records_count) {
		def_active_cell->Hide();
		active_cell_hidden = true;
		scroll_ended = true;
		return;
	}

	if (record >= new_records_count)
		active_record = new_records_count - 1;
	else
		active_record = record;

	OnActiveCellLocationChanged();
}

void CEditableTextCell::CommitChangesIfPresent() {

	if (changes_present) {
		const Tchar *value = def_active_cell->GetLabel();

		skip_reloading = true;
		table_proxy->SetCell(active_field, active_record, value);
		skip_reloading = false;

		changes_present = false;
	}
}

void CEditableTextCell::OnActiveCellLocationChanged() {

	def_active_cell->Hide();
	Reload();

	active_cell_hidden = true;
	scroll_ended = true;
}

void CEditableTextCell::Reload() {

	if (skip_reloading || !table_proxy->GetRecordsCount()) return;

	ImmutableString<Tchar> label = table_proxy->GetCellAsString(active_field, active_record);

	update_cell_widget_text = true;
	def_active_cell->SetLabel(label.str ? label.str : _T(""), label.size);
	update_cell_widget_text = false;
}