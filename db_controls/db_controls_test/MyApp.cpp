#include "MyApp.h"

IMPLEMENT_APP(CMyApp)

void CMyApp::OnInit(){

frame = nullptr;
frame = new CMyFrame(_T("DBGRID_TEST"), _T("DBGRID_TEST"),\
				   20, 20,\
				   800, 600);
}

CMyApp::~CMyApp(){

if(frame) delete frame;
}
