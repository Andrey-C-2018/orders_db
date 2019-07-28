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

std::shared_ptr<XEvent> XEvent::clone() const noexcept {

	return std::make_shared<XEvent>();
}

EvtPair XEvent::cloneAndCreateArgsWrapper() const noexcept {

	return cloneTemplate<XEvent>();
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

std::shared_ptr<XEvent> XCommandEvent::clone() const noexcept {

	return std::make_shared<XCommandEvent>();
}

EvtPair XCommandEvent::cloneAndCreateArgsWrapper() const noexcept {

	return cloneTemplate<XCommandEvent>();
}

XCommandEvent::~XCommandEvent(){ }

//****************************************************************************

XSizeEvent::XSizeEvent() noexcept : width(0), height(0), param(0) { }

std::shared_ptr<XEvent> XSizeEvent::clone() const noexcept {

	return std::make_shared<XSizeEvent>();
}

EvtPair XSizeEvent::cloneAndCreateArgsWrapper() const noexcept {

	return cloneTemplate<XSizeEvent>();
}

void XSizeEvent::PostInit(_plCallbackFnParams){

	width = _plGetWidth(_plCallbackFnParamsList);
	height = _plGetHeight(_plCallbackFnParamsList);
	param = _plGetSizeParam(_plCallbackFnParamsList);
}

XSizeEvent::~XSizeEvent(){ }

//****************************************************************************

XMoveEvent::XMoveEvent() noexcept : x(0), y(0) { }

std::shared_ptr<XEvent> XMoveEvent::clone() const noexcept {

	return std::make_shared<XMoveEvent>();
}

EvtPair XMoveEvent::cloneAndCreateArgsWrapper() const noexcept {

	return cloneTemplate<XMoveEvent>();
}

void XMoveEvent::PostInit(_plCallbackFnParams){

	x = _plGetX(_plCallbackFnParamsList);
	y = _plGetY(_plCallbackFnParamsList);
}

XMoveEvent::~XMoveEvent(){ }

//****************************************************************************

XMouseEvent::XMouseEvent() noexcept : x(0), y(0), virtual_key(0) { }

std::shared_ptr<XEvent> XMouseEvent::clone() const noexcept {

	return std::make_shared<XMouseEvent>();
}

EvtPair XMouseEvent::cloneAndCreateArgsWrapper() const noexcept {

	return cloneTemplate<XMouseEvent>();
}

void XMouseEvent::PostInit(_plCallbackFnParams){

	x = _plGetX(_plCallbackFnParamsList);
	y = _plGetY(_plCallbackFnParamsList);
	virtual_key = _plGetMouseVKey(_plCallbackFnParamsList);
}

XMouseEvent::~XMouseEvent(){ }

//****************************************************************************

XMouseWheelEvent::XMouseWheelEvent() noexcept : delta(0) { }

std::shared_ptr<XEvent> XMouseWheelEvent::clone() const noexcept {

	return std::make_shared<XMouseWheelEvent>();
}

EvtPair XMouseWheelEvent::cloneAndCreateArgsWrapper() const noexcept {

	return cloneTemplate<XMouseWheelEvent>();
}

void XMouseWheelEvent::PostInit(_plCallbackFnParams){

	delta = _plGetMouseWheelDelta(_plCallbackFnParamsList);
}

XMouseWheelEvent::~XMouseWheelEvent(){ }

//****************************************************************************

XKeyboardEvent::XKeyboardEvent() noexcept : ch(0), key(0){ }

std::shared_ptr<XEvent> XKeyboardEvent::clone() const noexcept {

	return std::make_shared<XKeyboardEvent>();
}

EvtPair XKeyboardEvent::cloneAndCreateArgsWrapper() const noexcept {

	return cloneTemplate<XKeyboardEvent>();
}

void XKeyboardEvent::PostInit(_plCallbackFnParams){

	ch = _plGetChar(_plCallbackFnParamsList);
	key = _plGetKey(_plCallbackFnParamsList);
}

XKeyboardEvent::~XKeyboardEvent(){ }

//****************************************************************************

XScrollEvent::XScrollEvent() noexcept : thumb_pos(0), scroll_type(0) { }

std::shared_ptr<XEvent> XScrollEvent::clone() const noexcept {

	return std::make_shared<XScrollEvent>();
}

EvtPair XScrollEvent::cloneAndCreateArgsWrapper() const noexcept {

	return cloneTemplate<XScrollEvent>();
}

void XScrollEvent::PostInit(_plCallbackFnParams){

	thumb_pos = _plGetScrollThumbPos(_plCallbackFnParamsList);
	scroll_type = _plGetScrollType(_plCallbackFnParamsList);
}

XScrollEvent::~XScrollEvent(){ }

//****************************************************************************

XScrollEventEx::XScrollEventEx() noexcept { }

std::shared_ptr<XEvent> XScrollEventEx::clone() const noexcept {

	return std::make_shared<XScrollEventEx>();
}

EvtPair XScrollEventEx::cloneAndCreateArgsWrapper() const noexcept {

	return cloneTemplate<XScrollEventEx>();
}

void XScrollEventEx::PostInit(_plCallbackFnParams) {

	thumb_pos = _plGetScrollThumbPosLarge(_plCallbackFnParamsList);
	scroll_type = _plGetScrollType(_plCallbackFnParamsList);
}

XScrollEventEx::~XScrollEventEx(){ }

//****************************************************************************

XWindowButtonsEvent::XWindowButtonsEvent() noexcept : button(-1) {

	ExecuteDefaultEventAction(true);
}

std::shared_ptr<XEvent> XWindowButtonsEvent::clone() const noexcept {

	return std::make_shared<XWindowButtonsEvent>();
}

EvtPair XWindowButtonsEvent::cloneAndCreateArgsWrapper() const noexcept {

	return cloneTemplate<XWindowButtonsEvent>();
}

void XWindowButtonsEvent::PostInit(_plCallbackFnParams) {

	button = _plGetWindowButtonType(_plCallbackFnParamsList);
}

XWindowButtonsEvent::~XWindowButtonsEvent() { }