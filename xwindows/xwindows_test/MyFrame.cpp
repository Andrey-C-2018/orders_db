#include "MyFrame.h"

CMyFrame::CMyFrame(const Tchar *class_name,\
					 const Tchar *label, const int X, const int Y,\
					 const int width, const int height) {
Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
		label, X, Y, width, height);

Connect(EVT_SIZE, this, &CMyFrame::OnSize);

XRect rc;
GetClientRect(rc);

InitTestControls(rc);
}

void CMyFrame::InitTestControls(const XRect &rc) {

label1 = new XLabel(this, FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
	_T("Calculator"), 20, 20, rc.right - 40, 30);
edit1 = new XEdit(this, FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
	_T(""), 20, 60, rc.right - 40, 30);

int gap_x = 20, gap_y = 20;
int button_width = (rc.right - 200 - gap_x * 4) / 3;
int button_height = (rc.bottom - gap_y * 4 - 100) / 4;

const Tchar labels[] = _T("789456123");

for (int i = 0; i < 3; ++i)
	for (int j = 0; j < 3; ++j) {
		Tchar label[2];

		label[0] = labels[3 * j + i];
		label[1] = _T('\0');
		number_buttons[3 * i + j] = new XButton(this, FL_WINDOW_VISIBLE, \
			label, \
			gap_x + (button_width + gap_x) * i, \
			100 + (button_height + gap_y) * j, \
			button_width, button_height);
		number_buttons[3 * i + j]->SetTag(label[0] - _T('0'));
		Connect(EVT_COMMAND, number_buttons[3 * i + j]->GetId(), \
			this, &CMyFrame::OnButtonNumberClick);
	}
number_buttons[9] = new XButton(this, FL_WINDOW_VISIBLE, \
	_T("0"), \
	gap_x + (button_width + gap_x), \
	100 + (button_height + gap_y) * 3, \
	button_width, button_height);
number_buttons[9]->SetTag(0);
Connect(EVT_COMMAND, number_buttons[9]->GetId(), this, &CMyFrame::OnButtonNumberClick);

button_ce = new XButton(this, FL_WINDOW_VISIBLE, \
	_T("CE"), \
	gap_x + (button_width + gap_x) * 3, \
	100, \
	button_width, button_height);
Connect(EVT_COMMAND, button_ce->GetId(), this, &CMyFrame::OnButtonCeClick);

actions = new XComboBox(this, FL_WINDOW_VISIBLE | FL_COMBOBOX_DROPDOWN, \
	_T(""), 600, 250, \
	80, 30 + 100);
actions->AddItem(_T("+"));
actions->AddItem(_T("-"));
actions->AddItem(_T("*"));
actions->AddItem(_T("/"));
}

void CMyFrame::OnButtonNumberClick(XCommandEvent *eve) {
size_t size(0);
label = edit1->GetLabel(size);

int button_value = eve->GetSender()->GetTag();
if (size == 1 && label[0] == _T('0')) {
	if (button_value == 0) return;
	else label.clear();
}

label.append(1, _T('0') + button_value);
edit1->SetLabel(label.c_str());
}

void CMyFrame::OnButtonCeClick(XCommandEvent *eve) {

edit1->SetLabel(_T(""));
actions->SetSelectionIndex(XComboBox::EMPTY_INDEX);
}

void CMyFrame::OnSize(XSizeEvent *eve) {
int width = eve->GetWidth();
int height = eve->GetHeight();

}

CMyFrame::~CMyFrame(){ }