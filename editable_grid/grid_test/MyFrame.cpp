#include "MyFrame.h"
#include <basic/XConv.h>
#include <editable_grid/ComboBoxCellWidget.h>

CMyFrame::CMyFrame(const Tchar *class_name,\
					 const Tchar *label, const int X, const int Y,\
					 const int width, const int height) : grid(nullptr){
	
	Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);

	Connect(EVT_SIZE, this, &CMyFrame::OnSize);

	XRect rc;
	GetClientRect(rc);

	grid = new CStringGrid();

	int sizes[14] = {3, 10, 5, 4, 2, 8, 15, 2, 1, 6, 25, 35, 2, 40};
	const size_t size = 8;
	Tchar buffer[30];
	for (size_t i = 0; i < size; ++i)
		grid->AddField(XConv::ToString(sizes[i], buffer), sizes[i]);

	for (size_t i = 0; i < 2*size; ++i)
		grid->AddRecord();

	for(size_t i = 0; i < size; ++i)
	for (size_t j = 0; j < 2*size; ++j)
		grid->SetCell(i, j, XConv::ToString(10 * i + j, buffer));

	grid->SetWidgetForField(3, new CComboBoxCellWidget());

	grid->Create(this, FL_WINDOW_VISIBLE, _T(""), 20, 40, \
					rc.right - 40, rc.bottom - 60);
	grid->SetFocus();

	button_focus = new XButton(this, FL_WINDOW_VISIBLE, \
								_T("Focus"), 20, 10, 50, 20);
	button_add = new XButton(this, FL_WINDOW_VISIBLE, \
								_T("Add"), 80, 10, 40, 20);
	button_remove = new XButton(this, FL_WINDOW_VISIBLE, \
								_T("Remove"), 130, 10, 65, 20);
	button_refresh = new XButton(this, FL_WINDOW_VISIBLE, \
								_T("Refresh"), 205, 10, 70, 20);

	Connect(EVT_COMMAND, button_focus->GetId(), this, \
			&CMyFrame::OnButtonFocusClick);
	Connect(EVT_COMMAND, button_add->GetId(), this, \
			&CMyFrame::OnButtonAddClick);
	Connect(EVT_COMMAND, button_remove->GetId(), this, \
			&CMyFrame::OnButtonRemoveClick);
	Connect(EVT_COMMAND, button_refresh->GetId(), this, \
			&CMyFrame::OnButtonRefreshClick);
}

void CMyFrame::OnSize(XSizeEvent *eve) {
	int width = eve->GetWidth();
	int height = eve->GetHeight();

	if (grid) grid->MoveWindow(20, 40, width - 40, height - 60);
}

void CMyFrame::OnButtonFocusClick(XCommandEvent *eve) {

	grid->SetFocus();
}

void CMyFrame::OnButtonAddClick(XCommandEvent *eve) { }

void CMyFrame::OnButtonRemoveClick(XCommandEvent *eve) { }

void CMyFrame::OnButtonRefreshClick(XCommandEvent *eve) { }

CMyFrame::~CMyFrame(){ }