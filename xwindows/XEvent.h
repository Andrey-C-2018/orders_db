#pragma once
#include "platform_specific.h"
#include <basic/Delegate.h>

class XWindow;

class XEvent{
	const _plEventId ID_event;
	const _plNotificationCode ID_ncode;
	XWindow *Sender;
	bool default_action;

	XEvent(const XEvent &obj) = delete;
	XEvent(XEvent &&obj) = delete;
	XEvent &operator=(const XEvent &obj) = delete;
	XEvent &operator=(XEvent &&obj) = delete;
protected:
	_plEventInfo event_info;
	XEvent(const _plEventId id_event, const _plNotificationCode id_ncode) noexcept;
	inline void SetSender(XWindow *sender) { Sender = sender; }
public:
	XEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
				XWindow *sender, const int id) noexcept;
	inline _plEventId GetEventType() const { return ID_event; }
	inline _plNotificationCode GetNCode() const { return ID_ncode; }
	inline XWindow *GetSender() const { return Sender; }
	virtual void PostInit(_plCallbackFnParams);
	void PropagateEvent(XWindow *receiver) const noexcept;
	inline void ExecuteDefaultEventAction(bool yes_no) { this->default_action = yes_no; }
	inline bool GetDefaultActionStatus() const { return default_action; }
	virtual ~XEvent();
};

class XCommandEvent : public XEvent{
public:
	XCommandEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
				XWindow *sender, const int id);
	virtual ~XCommandEvent();
};

class XSizeEvent : public XEvent{
	int width, height;
	int param;
public:
	XSizeEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
				XWindow *sender, const int id) noexcept;
	void PostInit(_plCallbackFnParams);
	inline int GetWidth() const{ return width; }
	inline int GetHeight() const{ return height; }
	inline int GetSizeParam() const{ return param; }
	virtual ~XSizeEvent();
};

class XMoveEvent : public XEvent{
	int x, y;
public:
	XMoveEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
				XWindow *sender, const int id) noexcept;
	void PostInit(_plCallbackFnParams);
	inline int GetX() const{ return x; }
	inline int GetY() const{ return y; }
	virtual ~XMoveEvent();
};

class XMouseEvent : public XEvent{
	int x, y;
	int virtual_key;
public:
	XMouseEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
				XWindow *sender, const int id) noexcept;
	void PostInit(_plCallbackFnParams);
	inline int GetX() const{ return x; }
	inline int GetY() const{ return y; }
	inline int GetVirtualKey() const{ return virtual_key; }
	virtual ~XMouseEvent();
};

class XMouseWheelEvent : public XMouseEvent{
	int delta;
public:
	XMouseWheelEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
				XWindow *sender, const int id) noexcept;
	void PostInit(_plCallbackFnParams);
	inline int GetDelta() const{ return delta; }
	virtual ~XMouseWheelEvent();
};

class XKeyboardEvent : public XEvent{
	Tchar ch;
	int key;
public:
	XKeyboardEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
				XWindow *sender, const int id) noexcept;
	void PostInit(_plCallbackFnParams);
	inline Tchar GetChar() const{ return ch; }
	inline Tchar GetKey() const{ return key; }
	inline int GetVKeyState(int virtual_key) noexcept{ return _plGetVKeyState(virtual_key); }
	virtual ~XKeyboardEvent();
};

class XScrollEvent : public XEvent{
protected:
	int thumb_pos;
	int scroll_type;
public:
	XScrollEvent(const _plEventId id_event, const _plNotificationCode id_ncode,\
				XWindow *sender, const int id) noexcept;
	void PostInit(_plCallbackFnParams);
	inline int GetThumbPos() const{ return thumb_pos; }
	inline int GetScrollType() const{ return scroll_type; }
	virtual ~XScrollEvent();
};

class XScrollEventEx : public XScrollEvent {
public:
	XScrollEventEx(const _plEventId id_event, const _plNotificationCode id_ncode, \
				XWindow *sender, const int id) noexcept;
	void PostInit(_plCallbackFnParams);
	virtual ~XScrollEventEx() noexcept;
};
