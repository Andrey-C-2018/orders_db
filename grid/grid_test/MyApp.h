#pragma once
#include <xwindows/XApp.h>
#include "MyFrame.h"

class CMyApp : public XApp{
	CMyFrame *frame;
public:
	void OnInit();
	virtual ~CMyApp();
	DECLARE_APP
};