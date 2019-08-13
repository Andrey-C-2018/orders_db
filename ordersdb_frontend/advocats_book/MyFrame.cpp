#include "MyFrame.h"
#include "AdvocatsBook.h"

CMyFrame::CMyFrame(const Tchar *class_name,\
					 const Tchar *label, const int X, const int Y,\
					 const int width, const int height) : main(nullptr) {
	
	Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);

	XRect rc;
	GetClientRect(rc);

	main = new CAdvocatsBook(this, 0, _T(""), 0, 0, rc.right, rc.bottom);
	Connect(EVT_SIZE, this, &CMyFrame::OnSize);
}

void CMyFrame::OnSize(XSizeEvent *eve) {

	main->OnSize(eve);
}

CMyFrame::~CMyFrame(){ }