#include "MyApp.h"

IMPLEMENT_APP(CMyApp)

void CMyApp::OnInit(){

	frame = nullptr;
	frame = new CAdvocatsBook(_T("ORDERSDB_ADV_REFERNCE"), \
					_T("Довідник адвокатів"), \
				   20, 10,\
				   1000, 650);
}

CMyApp::~CMyApp(){

	if(frame) delete frame;
}
