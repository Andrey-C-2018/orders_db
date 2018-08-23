#include "XEvent.h"
#include "XWindow.h"

XEvent::XEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
				XWindow *sender, const int id) noexcept: ID_event(id_event), \
													ID_ncode(id_ncode), \
													Sender(sender), \
													default_action(false){ }

XEvent::XEvent(const _plEventId id_event, const _plNotificationCode id_ncode) noexcept: \
													ID_event(id_event), \
													ID_ncode(id_ncode), \
													Sender(NULL), \
													default_action(false){ }

void XEvent::PostInit(_plCallbackFnParams){

event_info = _plInitEventInfo(_plCallbackFnParamsList);
}

void XEvent::PropagateEvent(XWindow *receiver) const noexcept {

_plSendMessage(receiver->GetInternalHandle(), event_info);
}

XEvent::~XEvent(){ }

//****************************************************************************

XCommandEvent::XCommandEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
							 XWindow *sender, const int id) : \
							 XEvent(id_event, id_ncode){ 

SetSender(sender->GetChildById(id));
// TO DO: change the exception class
if(!GetSender()){
	XEventHandlerException e(XEventHandlerException::E_WINDOW_NOT_EXIST,\
					_T("there are no window with such ID:"));
	e<< id<< _T(" ID_Event = ")<< id_event<< _T(", ID_NCode = ")<< id_ncode;
	throw e;
}
}

XCommandEvent::~XCommandEvent(){ }

//****************************************************************************

XSizeEvent::XSizeEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
							 XWindow *sender, const int id) noexcept : \
							 XEvent(id_event, id_ncode, sender, id){ }

void XSizeEvent::PostInit(_plCallbackFnParams){

event_info = _plInitEventInfo(_plCallbackFnParamsList);
width = _plGetWidth(_plCallbackFnParamsList);
height = _plGetHeight(_plCallbackFnParamsList);
param = _plGetSizeParam(_plCallbackFnParamsList);
}

XSizeEvent::~XSizeEvent(){ }

//****************************************************************************

XMoveEvent::XMoveEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
							 XWindow *sender, const int id) noexcept: \
							 XEvent(id_event, id_ncode, sender, id){ }

void XMoveEvent::PostInit(_plCallbackFnParams){

event_info = _plInitEventInfo(_plCallbackFnParamsList);
x = _plGetX(_plCallbackFnParamsList);
y = _plGetY(_plCallbackFnParamsList);
}

XMoveEvent::~XMoveEvent(){ }

//****************************************************************************

XMouseEvent::XMouseEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
							 XWindow *sender, const int id) noexcept: \
							 XEvent(id_event, id_ncode, sender, id){ }

void XMouseEvent::PostInit(_plCallbackFnParams){

event_info = _plInitEventInfo(_plCallbackFnParamsList);
x = _plGetX(_plCallbackFnParamsList);
y = _plGetY(_plCallbackFnParamsList);
virtual_key = _plGetMouseVKey(_plCallbackFnParamsList);
}

XMouseEvent::~XMouseEvent(){ }

//****************************************************************************

XMouseWheelEvent::XMouseWheelEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
							 XWindow *sender, const int id) noexcept: \
							 XMouseEvent(id_event, id_ncode, sender, id){ }

void XMouseWheelEvent::PostInit(_plCallbackFnParams){

event_info = _plInitEventInfo(_plCallbackFnParamsList);
delta = _plGetMouseWheelDelta(_plCallbackFnParamsList);
}

XMouseWheelEvent::~XMouseWheelEvent(){ }

//****************************************************************************

XKeyboardEvent::XKeyboardEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
							 XWindow *sender, const int id) noexcept: \
							 XEvent(id_event, id_ncode, sender, id), ch(0), key(0){ }

void XKeyboardEvent::PostInit(_plCallbackFnParams){

event_info = _plInitEventInfo(_plCallbackFnParamsList);
ch = _plGetChar(_plCallbackFnParamsList);
key = _plGetKey(_plCallbackFnParamsList);
}

XKeyboardEvent::~XKeyboardEvent(){ }

//****************************************************************************

XScrollEvent::XScrollEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
							 XWindow *sender, const int id) noexcept: \
							 XEvent(id_event, id_ncode, sender, id){ }

void XScrollEvent::PostInit(_plCallbackFnParams){

event_info = _plInitEventInfo(_plCallbackFnParamsList);
thumb_pos = _plGetScrollThumbPos(_plCallbackFnParamsList);
scroll_type = _plGetScrollType(_plCallbackFnParamsList);
}

XScrollEvent::~XScrollEvent(){ }

//****************************************************************************

XScrollEventEx::XScrollEventEx(const _plEventId id_event, const _plNotificationCode id_ncode, \
							XWindow *sender, const int id) noexcept: \
							XScrollEvent(id_event, id_ncode, sender, id) { }

void XScrollEventEx::PostInit(_plCallbackFnParams) {

event_info = _plInitEventInfo(_plCallbackFnParamsList);
thumb_pos = _plGetScrollThumbPosLarge(_plCallbackFnParamsList);
scroll_type = _plGetScrollType(_plCallbackFnParamsList);
}

XScrollEventEx::~XScrollEventEx(){ }