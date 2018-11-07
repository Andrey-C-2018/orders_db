#include "MyFrame.h"
#include <basic/XConv.h>

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
const size_t size = 14;
Tchar buffer[30];
for (size_t i = 0; i < size; ++i)
	grid->AddField(XConv::ToString(sizes[i], buffer), sizes[i]);

for (size_t i = 0; i < 2*size; ++i)
	grid->AddRecord();

for(size_t i = 0; i < size; ++i)
for (size_t j = 0; j < 2*size; ++j)
	grid->SetCell(i, j, XConv::ToString(10 * i + j, buffer));

grid->Create(this, FL_WINDOW_VISIBLE, _T(""), 20, 20, \
				rc.right - 40, rc.bottom - 40);
grid->SetFocus();

button_ce = new XButton(this, FL_WINDOW_VISIBLE, \
						_T("CE"), 10, 10, 10, 10);
Connect(EVT_COMMAND, button_ce->GetId(), this, &CMyFrame::OnButtonCeClick);
}

void CMyFrame::OnButtonCeClick(XCommandEvent *eve) {

	grid->SetFocus();
}

void CMyFrame::OnSize(XSizeEvent *eve) {
int width = eve->GetWidth();
int height = eve->GetHeight();

if (grid) grid->MoveWindow(20, 20, width - 40, height - 40);
}

CMyFrame::~CMyFrame(){ }