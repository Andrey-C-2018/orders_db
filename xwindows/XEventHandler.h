#pragma once
#include <vector>
#include <algorithm>
#include <basic/Delegate.h>
#include <basic/Exception.h>
#include <basic/stdafx.h>
#include "XEvent.h"

class XEventHandlerException : public XException{
public:
	enum Errors{
		E_WRONG_OBJECT = 1,\
		E_WRONG_EVENT_TYPE = 2,\
		E_WINDOW_NOT_EXIST = 3,\
		E_OVERRIDE_STATEMENTS = 4
	};
	enum Warnings{
		W_DUPLICATE_HANDLER = RESULT_FAIL,\
		W_HANDLER_NOT_FOUND = RESULT_FAIL
	};
	XEventHandlerException(const int err_code, const Tchar *err_descr);
	XEventHandlerException(const XEventHandlerException &obj);
	XEventHandlerException(XEventHandlerException &&obj) = default;
	~XEventHandlerException();
};

//****************************************************************************

class XEventHandlerBasic{
protected:
	struct CEvtHandlerRec{
		_plEventId id_event;
		CDelegate evt_handler_caller;
		XEvent *eve;
		IArguments *eve_container;
		inline bool operator==(const CEvtHandlerRec &obj) const{

			return id_event == obj.id_event;
		}
		inline bool operator<(const CEvtHandlerRec &obj) const{

			return id_event < obj.id_event;
		}
		CEvtHandlerRec() noexcept : id_event(0), eve(nullptr), \
									eve_container(nullptr){ }

		CEvtHandlerRec(const CEvtHandlerRec &obj) = delete;

		CEvtHandlerRec(CEvtHandlerRec &&obj) noexcept {
			this->id_event = obj.id_event;  obj.id_event = 0;
			this->evt_handler_caller = std::move(obj.evt_handler_caller);
			this->eve = obj.eve;  obj.eve = nullptr;
			this->eve_container = obj.eve_container; obj.eve_container = nullptr;
		}

		CEvtHandlerRec &operator=(const CEvtHandlerRec &obj) = delete;

		inline CEvtHandlerRec &operator=(CEvtHandlerRec &&obj) noexcept {
			this->id_event = obj.id_event;  obj.id_event = 0;
			this->evt_handler_caller = std::move(obj.evt_handler_caller);
			this->eve = obj.eve;  obj.eve = nullptr;
			this->eve_container = obj.eve_container; obj.eve_container = nullptr;
			return *this;
		}

	};
	struct CEvtHandlerRecEx : public CEvtHandlerRec{
		_plHWND hwnd;
		int id;
		_plNotificationCode id_ncode;

		inline bool operator==(const CEvtHandlerRecEx &obj) const{

			return hwnd == obj.hwnd && id == obj.id && \
					id_event == obj.id_event && id_ncode == obj.id_ncode;
		}
		inline bool operator<(const CEvtHandlerRecEx &obj) const{
			
			return hwnd < obj.hwnd || \
					(hwnd == obj.hwnd && id < obj.id) || \
					(hwnd == obj.hwnd && id == obj.id && id_event < obj.id_event) || \
					(hwnd == obj.hwnd && id == obj.id && id_event == obj.id_event && \
							id_ncode < obj.id_ncode);
		}
		CEvtHandlerRecEx() noexcept : CEvtHandlerRec(), id(-1), id_ncode(-1) { _plUnsetHWND(hwnd); }

		CEvtHandlerRecEx(const CEvtHandlerRecEx &obj) = delete;

		CEvtHandlerRecEx(CEvtHandlerRecEx &&obj) noexcept : \
												CEvtHandlerRec(std::move(obj)), \
												hwnd(obj.hwnd), id(obj.id), \
												id_ncode(obj.id_ncode) {
			_plUnsetHWND(obj.hwnd);
			obj.id = -1;
			obj.id_ncode = -1;
		}

		CEvtHandlerRecEx &operator=(const CEvtHandlerRecEx &obj) = delete;

		CEvtHandlerRecEx &operator=(CEvtHandlerRecEx &&obj) noexcept{
			CEvtHandlerRec::operator=(std::move(obj));
			hwnd = obj.hwnd; _plUnsetHWND(obj.hwnd);
			id = obj.id; obj.id = -1;
			id_ncode = obj.id_ncode;
			return *this;
		}
	};

	typedef std::vector<CEvtHandlerRec>::iterator CEvtHandlerIterator;
	typedef std::vector<CEvtHandlerRec>::const_iterator CEvtHandlerConstIterator;
	typedef std::vector<CEvtHandlerRecEx>::iterator CEvtHandlerIteratorEx;
	typedef std::vector<CEvtHandlerRecEx>::const_iterator CEvtHandlerConstIteratorEx;
};

#define DECLARE_EVENT_OVERRIDE \
private: \
	static XEventHandlerEmbedded **p_event_handler; \
	static _plCallbackFnRetValue _plCallbackFnModifier OverrideFunc(_plCallbackFnParams){ \
		return (*p_event_handler)->WndProc(_plCallbackFnParamsList); \
	} \
protected: \
	XEventHandlerEmbedded *InitEventHandlerEmbedded();

#define IMPLEMENT_EVENT_OVERRIDE(XWindowDerived) \
XEventHandlerEmbedded **XWindowDerived::p_event_handler = nullptr; \
XEventHandlerEmbedded *XWindowDerived::InitEventHandlerEmbedded(){ \
	if(!event_handler_embedded){ \
		p_event_handler = &event_handler_embedded; \
		event_handler_embedded = new XEventHandlerEmbedded(this, OverrideFunc); \
	} \
	return *p_event_handler; \
}

class XEventHandler;

class XEventHandlerEmbedded : public XEventHandlerBasic{
	std::vector<CEvtHandlerRec> evt_handlers;
	XWindow *window;
	_plWNDPROC orig_wnd_proc;
public:
	XEventHandlerEmbedded(XWindow *wnd, _plWNDPROC new_wnd_proc);
	XEventHandlerEmbedded(const XEventHandlerEmbedded &obj) = delete;
	XEventHandlerEmbedded(XEventHandlerEmbedded &&obj) = default;
	XEventHandlerEmbedded &operator=(const XEventHandlerEmbedded &obj) = delete;
	XEventHandlerEmbedded &operator=(XEventHandlerEmbedded &&obj) = default;
	inline _plCallbackFnRetValue _plCallbackFnModifier \
		WndProc(_plCallbackFnParams);

	template <class TObject, class TEvent> \
		int Connect(const _plEventId id_event,\
					TObject *obj, \
					void (TObject::*PFunc)(TEvent *));
	int Disconnect(const _plEventId id_event);
	void DisconnectAll();
	virtual ~XEventHandlerEmbedded();
};

class XEventHandler : public XEventHandlerBasic{
	_plHWND hwnd;
	int id;
	static std::vector<CEvtHandlerRecEx> evt_handlers;

	static int max_id;
protected:
	XEventHandlerEmbedded *event_handler_embedded;
	virtual XEventHandlerEmbedded *InitEventHandlerEmbedded(){
		
		throw XEventHandlerException(XEventHandlerException::E_OVERRIDE_STATEMENTS,\
									_T("you forgot to define DECLARE_EVENT_OVERRIDE and/or IMPLEMENT_EVENT_OVERRIDE for the class"));
		return nullptr;
	}

	static _plCallbackFnRetValue _plCallbackFnModifier \
		MainWndProc(_plCallbackFnParams);
	template <class TObject, class TEvent> \
		inline int ConnectImpl(const _plEventId id_event,\
								const _plNotificationCode id_ncode,\
								const int id,\
								TObject *obj, \
								void (TObject::*PFunc)(TEvent *));
	inline void SetInternalHandle(const _plHWND hwnd) { this->hwnd = hwnd; }
	virtual void OnWindowCreationCompleted() = 0;
	virtual XWindow *GetThisAsXWindow() = 0;

	inline int CreateUniqueId() { 
		
		if(id == -1) id = max_id++;
		return id;
	}
	inline int SetIdToNull() { 
		if (id == -1) id = 0; 
		return id; 
	}
	virtual int InitializeId() = 0;
public:
	XEventHandler();
	XEventHandler(const XEventHandler &obj) = delete;
	XEventHandler(XEventHandler &&obj);
	XEventHandler &operator=(const XEventHandler &obj) = delete;
	XEventHandler &operator=(XEventHandler &&obj);
	inline _plHWND GetInternalHandle() const{ return hwnd; }
	inline int GetId() const { return id; }
	template <class TObject, class TEvent> \
		int Connect(const _plEventId id_event,\
								const _plNotificationCode id_ncode,\
								const int id,\
								TObject *obj, \
								void (TObject::*PFunc)(TEvent *));
	template <class TObject, class TEvent> \
		int Connect(const _plEventId id_event,\
								const int id,\
								TObject *obj, \
								void (TObject::*PFunc)(TEvent *));
	template <class TObject, class TEvent> \
		int Connect(const _plEventId id_event,\
								TObject *obj, \
								void (TObject::*PFunc)(TEvent *));
	template <class TObject, class TEvent> \
		inline int OverrideWindowEvent(const _plEventId id_event,\
								TObject *obj, \
								void (TObject::*PFunc)(TEvent *));
	int Disconnect(_plHWND hwnd, const _plEventId id_event, const _plNotificationCode id_ncode, \
					const int id);
	int Disconnect(_plHWND hwnd, const _plEventId id_event, const int id);
	int Disconnect(_plHWND hwnd);
	static void DisconnectAll();
	virtual ~XEventHandler();
};

//****************************************************************************

_plCallbackFnRetValue _plCallbackFnModifier \
XEventHandlerEmbedded::WndProc(_plCallbackFnParams){
CEvtHandlerRec rec;
CEvtHandlerIterator p;

rec.id_event = _plGetEventId(_plCallbackFnParamsList);

p = std::find(evt_handlers.begin(), evt_handlers.end(), rec);

if(!(p != evt_handlers.end() && *p == rec))
	return _plDefaultEventAction(orig_wnd_proc, _plCallbackFnParamsList);

p->eve->PostInit(_plCallbackFnParamsList);

p->evt_handler_caller.Call(p->eve_container);
if(p->eve->GetDefaultActionStatus()){
	p->eve->ExecuteDefaultEventAction(false);
	return _plDefaultEventAction(orig_wnd_proc, _plCallbackFnParamsList);
}

return EVT_DONT_PROCESS;
}

template <class TObject, class TEvent> \
int XEventHandlerEmbedded::Connect(const _plEventId id_event,\
									TObject *obj, \
									void (TObject::*PFunc)(TEvent *)){
CEvtHandlerRec rec;
CEvtHandlerConstIterator p;

if (id_event == EVT_DESTROY) {
	throw XEventHandlerException(XEventHandlerException::E_WRONG_EVENT_TYPE, \
		_T("the destroy event is restricted to connect. Use the dtor instead"));
}

rec.id_event = id_event;

p = std::find(evt_handlers.cbegin(), evt_handlers.cend(), rec);
if(p != evt_handlers.cend() && *p == rec)
	return XEventHandlerException::W_DUPLICATE_HANDLER;

TEvent *eve = new TEvent(id_event, 0, window, 0);
rec.eve = dynamic_cast<XEvent*>(eve);
if(!rec.eve){
	if (eve) delete eve;
	XEventHandlerException e(XEventHandlerException::E_WRONG_EVENT_TYPE,\
					_T("the member function pointer argument is not inherited from XEvent: ID = "));
	e << window->GetId()<< _T(", ID_Event = ")<< id_event;
	throw e;
}
rec.evt_handler_caller.Connect(obj, PFunc);
rec.eve_container = new CArgumentsOne<TEvent>(eve);

evt_handlers.emplace_back(std::move(rec));
return RESULT_SUCCESS;
}

//****************************************************************************

template <class TObject, class TEvent> 
		int XEventHandler::ConnectImpl(const _plEventId id_event,\
								const _plNotificationCode id_ncode,\
								const int id,\
								TObject *obj, \
								void (TObject::*PFunc)(TEvent *)){
CEvtHandlerRecEx rec;
CEvtHandlerConstIteratorEx p;

assert(id >= 0);

XWindow *window = this->GetThisAsXWindow();

rec.hwnd = this->GetInternalHandle();
rec.id = (id_event == EVT_CREATE) ? InitializeId() : id;
rec.id_event = id_event;
rec.id_ncode = id_ncode;

p = std::lower_bound(evt_handlers.cbegin(), evt_handlers.cend(), rec);
if (p != evt_handlers.cend() && *p == rec) 
	return XEventHandlerException::W_DUPLICATE_HANDLER;

TEvent *eve = new TEvent(id_event, id_ncode, window, rec.id);
rec.eve = dynamic_cast<XEvent*>(eve);
if(!rec.eve){
	if(eve) delete eve;

	XEventHandlerException e(XEventHandlerException::E_WRONG_EVENT_TYPE,\
					_T("the member function pointer argument is not inherited from XEvent: ID = "));
	e << rec.id<< _T(", ID_Event = ")<< id_event<< _T(", ID_NCODE = ")<< id_ncode;
	throw e;
}

rec.evt_handler_caller.Connect(obj, PFunc);
rec.eve_container = new CArgumentsOne<TEvent *>(eve);

evt_handlers.emplace_back(std::move(rec));
std::sort(evt_handlers.begin(), evt_handlers.end());
return RESULT_SUCCESS;
}

template <class TObject, class TEvent> \
		int XEventHandler::Connect(const _plEventId id_event,\
								const _plNotificationCode id_ncode,\
								const int id,\
								TObject *obj, \
								void (TObject::*PFunc)(TEvent *)){

return ConnectImpl(id_event, id_ncode, id, obj, PFunc);
}

template <class TObject, class TEvent> \
		int XEventHandler::Connect(const _plEventId id_event,\
								const int id,\
								TObject *obj, \
								void (TObject::*PFunc)(TEvent *)){

return ConnectImpl(id_event, 0, id, obj, PFunc);
}

template <class TObject, class TEvent> \
		int XEventHandler::Connect(const _plEventId id_event,\
								TObject *obj, \
								void (TObject::*PFunc)(TEvent *)){

return ConnectImpl(id_event, 0, 0, obj, PFunc);
}

template <class TObject, class TEvent> \
int XEventHandler::OverrideWindowEvent(const _plEventId id_event,\
										TObject *obj, \
										void (TObject::*PFunc)(TEvent *)){

XEventHandlerEmbedded *event_handler_embedded = InitEventHandlerEmbedded();
return event_handler_embedded->Connect(id_event, obj, PFunc);
}

