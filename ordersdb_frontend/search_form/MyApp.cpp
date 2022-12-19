#include "MyApp.h"
#include <basic/locale_init.h>
#include <xwindows/platform_specific.h>

/*#ifdef _DEBUG
#include <vld.h>
#endif*/

IMPLEMENT_APP(CMyApp)

void CMyApp::OnInit(){

	setLocaleToCurrOS_Locale();

	frame = nullptr;
	XSize screen_size = _plGetScreenSize();

	frame = new CMyFrame(_T("ORDERSDB_SEARCH_FORM"), \
					_T("ordersdb: Пошук"), \
				   5, 0,\
					screen_size.width - 10, screen_size.height - 50);
}

CMyApp::~CMyApp(){

	if(frame) delete frame;
}
