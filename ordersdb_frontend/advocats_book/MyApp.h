#pragma once
#include <xwindows/XApp.h>
#include "AdvocatsBook.h"

class CMyApp : public XApp{
	CAdvocatsBook *frame;
public:
	void OnInit();
	virtual ~CMyApp();

	DECLARE_APP
};