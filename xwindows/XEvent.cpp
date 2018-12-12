#include "XEvent.h"
#include "XWindow.h"

XEvent::XEvent() noexcept: ID_event(0), ID_ncode(0), \
							Sender(nullptr), default_action(false) { }

void XEvent::InitEventData(const _plEventId id_event, \
							const _plNotificationCode id_ncode, \
							XWindow *sender, const int id) {

	assert(ID_event == 0);
	assert(ID_ncode == 0);
	assert(Sender == nullptr);

	ID_event = id_event;
	ID_ncode = id_ncode;
	Sender = sender;
}

void XEvent::PostInit(_plCallbackFnParams){ }

XEvent::~XEvent(){ }

//****************************************************************************

XCommandEvent::XCommandEvent() noexcept { }

void XCommandEvent::InitEventData(const _plEventId id_event, \
							const _plNotificationCode id_ncode, \
							XWindow *sender, const int id) {

	XEvent::InitEventData(id_event, id_ncode, sender, id);
	SetSender(sender->GetChildById(id));

	if(!GetSender()){
		XEventHandlerException e(XEventHandlerException::E_WINDOW_NOT_EXIST,\
								_T("there are no window with such ID:"));
		e << id << _T(" ID_Event = ") << id_event;
		e << _T(", ID_NCode = ") << id_ncode;
		throw e;
	}
}

XCommandEvent::~XCommandEvent(){ }

//****************************************************************************

XSizeEvent::XSizeEvent() noexcept : width(0), height(0), param(0) { }

void XSizeEvent::PostInit(_plCallbackFnParams){

	width = _plGetWidth(_plCallbackFnParamsList);
	height = _plGetHeight(_plCallbackFnParamsList);
	param = _plGetSizeParam(_plCallbackFnParamsList);
}

XSizeEvent::~XSizeEvent(){ }

//****************************************************************************

XMoveEvent::XMoveEvent() noexcept : x(0), y(0) { }

void XMoveEvent::PostInit(_plCallbackFnParams){

	x = _plGetX(_plCallbackFnParamsList);
	y = _plGetY(_plCallbackFnParamsList);
}

XMoveEvent::~XMoveEvent(){ }

//****************************************************************************

XMouseEvent::XMouseEvent() noexcept : x(0), y(0), virtual_key(0) { }

void XMouseEvent::PostInit(_plCallbackFnParams){

	x = _plGetX(_plCallbackFnParamsList);
	y = _plGetY(_plCallbackFnParamsList);
	virtual_key = _plGetMouseVKey(_plCallbackFnParamsList);
}

XMouseEvent::~XMouseEvent(){ }

//****************************************************************************

XMouseWheelEvent::XMouseWheelEvent() noexcept : delta(0) { }

void XMouseWheelEvent::PostInit(_plCallbackFnParams){

	delta = _plGetMouseWheelDelta(_plCallbackFnParamsList);
}

XMouseWheelEvent::~XMouseWheelEvent(){ }

//****************************************************************************

XKeyboardEvent::XKeyboardEvent() noexcept : ch(0), key(0){ }

void XKeyboardEvent::PostInit(_plCallbackFnParams){

	ch = _plGetChar(_plCallbackFnParamsList);
	key = _plGetKey(_plCallbackFnParamsList);
}

XKeyboardEvent::~XKeyboardEvent(){ }

//****************************************************************************

XScrollEvent::XScrollEvent() noexcept : thumb_pos(0), scroll_type(0) { }

void XScrollEvent::PostInit(_plCallbackFnParams){

	thumb_pos = _plGetScrollThumbPos(_plCallbackFnParamsList);
	scroll_type = _plGetScrollType(_plCallbackFnParamsList);
}

XScrollEvent::~XScrollEvent(){ }

//****************************************************************************

XScrollEventEx::XScrollEventEx() noexcept { }

void XScrollEventEx::PostInit(_plCallbackFnParams) {

	thumb_pos = _plGetScrollThumbPosLarge(_plCallbackFnParamsList);
	scroll_type = _plGetScrollType(_plCallbackFnParamsList);
}

XScrollEventEx::~XScrollEventEx(){ }