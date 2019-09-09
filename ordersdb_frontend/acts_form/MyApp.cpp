#include "MyApp.h"
#include <basic/locale_init.h>
#include <xwindows/platform_specific.h>

IMPLEMENT_APP(CMyApp)

void CMyApp::OnInit(){

	setLocaleToCurrOS_Locale();

	frame = nullptr;
	XSize screen_size = _plGetScreenSize();

	frame = new CMyFrame(_T("ORDERSDB_ACTS_FORM"), \
					_T("Форма внесення актів"), \
				   5, 0,\
					screen_size.width - 10, screen_size.height - 50);
}

CMyApp::~CMyApp(){

	if(frame) delete frame;
}
