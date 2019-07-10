#pragma once
#include <xwindows/XApp.h>
#include "ActsForm.h"

class CMyApp : public XApp{
	CActsForm *frame;
public:
	void OnInit();
	virtual ~CMyApp();

	DECLARE_APP
};