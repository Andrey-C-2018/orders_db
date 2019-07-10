#include "MyApp.h"

IMPLEMENT_APP(CMyApp)

void CMyApp::OnInit(){

	frame = nullptr;
	frame = new CActsForm(_T("ORDERSDB_ACTS_FORM"), \
					_T("Форма внесення актів"), \
				   20, 10,\
				   1000, 650);
}

CMyApp::~CMyApp(){

	if(frame) delete frame;
}
