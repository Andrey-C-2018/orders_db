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
									changes_present(false) { }

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

	ImmutableString<Tchar> label = table_proxy->GetCellAsString(active_field, active_record);
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

	if (changes_present) {
		const Tchar *value = def_active_cell->GetLabel();

		table_proxy->SetCell(active_field, active_record, value);
		changes_present = false;
	}

	active_field = field;
	active_record = record;

	def_active_cell->Hide();
	update_cell_widget_text = true;
	ImmutableString<Tchar> label = table_proxy->GetCellAsString(field, record);
	def_active_cell->SetLabel(label.str ? label.str : _T(""), label.size);
	update_cell_widget_text = false;

	active_cell_hidden = true;
	scroll_ended = true;
}