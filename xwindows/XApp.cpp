#include "XApp.h"
#include "platform_specific.h"
#include "XEventHandler.h"

XApp *XApp::instance = nullptr;

XApp *XApp::GetInstance(){

	if(!instance)
		instance = new XApp();
	return instance;
}

void XApp::OnInit(){ }

int XApp::LaunchMessageLoop() {

	if (!msg_loop_status) {
		msg_loop_status = true;
		return_code = _plLaunchMessageLoop();
		return return_code;
	}
	return RESULT_FAIL;
}

void XApp::Destroy(){

	if(instance) delete instance;
}

XApp::~XApp() noexcept { }

//****************************************************************************

int _plMAIN_FN_MODIFIERS _plMainEntryPoint(_plMainFnParams){
	int ret_value = RESULT_SUCCESS;

	_plInit(_plMainFnParamsList);
	XApp *app = XApp::GetInstance();

	try{
		app->OnInit();
		ret_value = app->LaunchMessageLoop();
	} 
	catch(XException &e){
		ErrorBox(e.what());
		ret_value = RESULT_FAIL;
	}
	catch(std::exception &e){
		ErrorBoxANSI(e.what());
		ret_value = RESULT_FAIL;
	}
	catch (...) {
		ErrorBox(_T("Undefined exception"));
		ret_value = RESULT_FAIL;
	}

	XApp::Destroy();
	_plPlatformSpecificCleanup();

	XEventHandler::DisconnectAll();
	CGlobalParams::Destroy();
	return ret_value;
}
