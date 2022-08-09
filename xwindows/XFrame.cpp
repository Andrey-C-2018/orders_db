#include <assert.h>
#include "XFrame.h"
#include "XApp.h"

XFrame::XFrame() noexcept{ }

XFrame::XFrame(const Tchar *class_name, const int flags, \
					 const Tchar *label, const int x, const int y,\
					 const int width, const int height){

Create(class_name, flags, label, x, y, width, height);
}

void XFrame::Create(const Tchar *class_name, const int flags, \
					 const Tchar *label, const int x, const int y,\
					 const int width, const int height){
	
CheckAndInitWindowClassName(class_name, CRegisterPredicate(*this, class_name));

	CreateImpl(nullptr, GetClassName().c_str(), flags, label, \
					x, y, width, height, \
					[](_plHWND hwnd_parent, const Tchar *class_name, \
						const int flags, const int X, const int Y, \
						const int width, const int height, \
						const Tchar *label, const int id, \
						XWindow *this_window) -> _plHWND {
	
						return _plCreateWindow(class_name, \
							flags, X, Y, width, height, \
							label, this_window);
					});

	int is_registered = Connect(EVT_DESTROY, this, &XFrame::OnDestroy) != \
									XEventHandlerException::W_DUPLICATE_HANDLER;
	assert(is_registered != RESULT_SUCCESS);

	_plWindowPostCreate(GetInternalHandle());
}

void XFrame::OnCreate(XEvent *eve) { }

void XFrame::OnDestroy(XEvent *eve) {

	if (IsCreated()) {
		send_destroy_event = false;
		Destroy();
	}

	XApp *app = XApp::GetInstance();
	if(!app->IsQuitMessagePosted())
		PostQuitMessage(RESULT_SUCCESS);
}

XFrame::~XFrame() {
	XApp *app = XApp::GetInstance();
	bool do_not_skip = app->IsMessageLoopLaunched();

	send_destroy_event = send_destroy_event && do_not_skip;
}
