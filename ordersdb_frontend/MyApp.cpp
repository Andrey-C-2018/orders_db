#include "MyApp.h"

IMPLEMENT_APP(CMyApp)

void CMyApp::OnInit(){

	frame = nullptr;
	frame = new CAdvocatsBook(_T("База даних доручень адвокатам"), \
					_T("ORDERSDB_FRONTEND"), \
				   20, 20,\
				   800, 600);
}

CMyApp::~CMyApp(){

	if(frame) delete frame;
}
