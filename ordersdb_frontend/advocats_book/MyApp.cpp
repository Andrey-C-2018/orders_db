#include "MyApp.h"
#include <xwindows/platform_specific.h>

IMPLEMENT_APP(CMyApp)

void CMyApp::OnInit(){

	frame = nullptr;
	XSize screen_size = _plGetScreenSize();

	frame = new CMyFrame(_T("ORDERSDB_ADV_REFERNCE"), \
					_T("Довідник адвокатів"), \
					5, 0, \
					screen_size.width - 10, screen_size.height - 50);
}

CMyApp::~CMyApp(){

	if(frame) delete frame;
}
