#pragma once
#include "platform_specific.h"
#include <basic/Delegate.h>

class XWindow;
class XEvent;
struct EvtPair final {
	std::shared_ptr<XEvent> eve;
	std::shared_ptr<IArguments> args;

	inline EvtPair() noexcept { }
	inline EvtPair(const EvtPair &obj) = default;
	inline EvtPair(EvtPair &&obj) = default;
	inline EvtPair &operator=(const EvtPair &obj) = default;
	inline EvtPair &operator=(EvtPair &&obj) = default;
	inline ~EvtPair() { }
};

class XEvent{
	_plEventId ID_event;
	_plNotificationCode ID_ncode;
	XWindow *Sender;
	bool default_action;

	XEvent(const XEvent &obj) = delete;
	XEvent(XEvent &&obj) = delete;
	XEvent &operator=(const XEvent &obj) = delete;
	XEvent &operator=(XEvent &&obj) = delete;
protected:

	inline void SetSender(XWindow *sender) { Sender = sender; }
	template <class TEvent> \
		inline EvtPair cloneTemplate() const noexcept {

		EvtPair pair;
		auto eve = std::make_shared<TEvent>();
		pair.args = std::make_shared<CArgumentsOne<TEvent *> >(eve.get());
		pair.eve = std::move(eve);

		return std::move(pair);
	}
public:
	XEvent() noexcept;
	virtual void InitEventData(const _plEventId id_event, \
							const _plNotificationCode id_ncode, \
							XWindow *sender, const int id);

	virtual std::shared_ptr<XEvent> clone() const noexcept;
	virtual EvtPair cloneAndCreateArgsWrapper() const noexcept;

	inline _plEventId GetEventType() const { return ID_event; }
	inline _plNotificationCode GetNCode() const { return ID_ncode; }
	inline XWindow *GetSender() const { return Sender; }

	virtual void PostInit(_plCallbackFnParams);

	inline void ExecuteDefaultEventAction(bool yes_no) { this->default_action = yes_no; }
	inline bool GetDefaultActionStatus() const { return default_action; }

	virtual ~XEvent();
};

class XCommandEvent : public XEvent{
public:
	XCommandEvent() noexcept;
	void InitEventData(const _plEventId id_event, \
						const _plNotificationCode id_ncode, \
						XWindow *sender, const int id) override;

	std::shared_ptr<XEvent> clone() const noexcept override;
	EvtPair cloneAndCreateArgsWrapper() const noexcept override;

	virtual ~XCommandEvent();
};

class XSizeEvent : public XEvent{
	int width, height;
	int param;
public:
	XSizeEvent() noexcept;

	std::shared_ptr<XEvent> clone() const noexcept override;
	EvtPair cloneAndCreateArgsWrapper() const noexcept override;

	void PostInit(_plCallbackFnParams) override;

	inline int GetWidth() const{ return width; }
	inline int GetHeight() const{ return height; }
	inline int GetSizeParam() const{ return param; }

	virtual ~XSizeEvent();
};

class XMoveEvent : public XEvent{
	int x, y;
public:
	XMoveEvent() noexcept;

	std::shared_ptr<XEvent> clone() const noexcept override;
	EvtPair cloneAndCreateArgsWrapper() const noexcept override;

	void PostInit(_plCallbackFnParams) override;

	inline int GetX() const{ return x; }
	inline int GetY() const{ return y; }

	virtual ~XMoveEvent();
};

class XMouseEvent : public XEvent{
	int x, y;
	int virtual_key;
public:
	XMouseEvent() noexcept;

	std::shared_ptr<XEvent> clone() const noexcept override;
	EvtPair cloneAndCreateArgsWrapper() const noexcept override;

	void PostInit(_plCallbackFnParams);

	inline int GetX() const{ return x; }
	inline int GetY() const{ return y; }
	inline int GetVirtualKey() const{ return virtual_key; }

	virtual ~XMouseEvent();
};

class XMouseWheelEvent : public XMouseEvent{
	int delta;
public:
	XMouseWheelEvent() noexcept;

	std::shared_ptr<XEvent> clone() const noexcept override;
	EvtPair cloneAndCreateArgsWrapper() const noexcept override;

	void PostInit(_plCallbackFnParams) override;

	inline int GetDelta() const{ return delta; }

	virtual ~XMouseWheelEvent();
};

class XKeyboardEvent : public XEvent{
	Tchar ch;
	int key;
public:
	XKeyboardEvent() noexcept;

	std::shared_ptr<XEvent> clone() const noexcept override;
	EvtPair cloneAndCreateArgsWrapper() const noexcept override;

	void PostInit(_plCallbackFnParams) override;

	inline Tchar GetChar() const{ return ch; }
	inline Tchar GetKey() const{ return key; }
	inline int GetVKeyState(int virtual_key) noexcept{ return _plGetVKeyState(virtual_key); }
	inline void SetKey(const int key) { this->key = key; }
	
	virtual ~XKeyboardEvent();
};

class XScrollEvent : public XEvent{
protected:
	int thumb_pos;
	int scroll_type;
public:
	XScrollEvent() noexcept;

	std::shared_ptr<XEvent> clone() const noexcept override;
	EvtPair cloneAndCreateArgsWrapper() const noexcept override;

	void PostInit(_plCallbackFnParams) override;

	inline int GetThumbPos() const{ return thumb_pos; }
	inline int GetScrollType() const{ return scroll_type; }

	virtual ~XScrollEvent();
};

class XScrollEventEx : public XScrollEvent {
public:
	XScrollEventEx() noexcept;

	std::shared_ptr<XEvent> clone() const noexcept override;
	EvtPair cloneAndCreateArgsWrapper() const noexcept override;

	void PostInit(_plCallbackFnParams) override;

	virtual ~XScrollEventEx() noexcept;
};

class XWindowButtonsEvent : public XEvent {
protected:
	unsigned button;
public:
	XWindowButtonsEvent() noexcept;

	std::shared_ptr<XEvent> clone() const noexcept override;
	EvtPair cloneAndCreateArgsWrapper() const noexcept override;

	void PostInit(_plCallbackFnParams) override;

	inline unsigned GetWindowButtonType() const noexcept { return button; }

	virtual ~XWindowButtonsEvent();
};
