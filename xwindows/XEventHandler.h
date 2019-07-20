#pragma once
#include <vector>
#include <algorithm>
#include <basic/Delegate.h>
#include <basic/Exception.h>
#include "platform_specific.h"
#include "XEvent.h"

class XEventHandlerException : public XException{
public:
	enum Errors{
		E_WRONG_OBJECT = 1,\
		E_WRONG_EVENT_TYPE = 2,\
		E_WINDOW_NOT_EXIST = 3,\
		E_DUPLICATE_HANDLER = 4
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

class XEventHandlerData final {
	CDelegate evt_handler_caller;
	std::shared_ptr<XEvent> eve;
	std::shared_ptr<IArguments> args_container;

	inline void copy(const XEventHandlerData &obj) {

		EvtPair pair = obj.eve->cloneAndCreateArgsWrapper();
		this->eve = std::move(pair.eve);
		args_container = std::move(pair.args);

		evt_handler_caller = obj.evt_handler_caller;
	}
public:
	XEventHandlerData() { }

	template <class TObject, class TEvent> \
		XEventHandlerData(TObject *object, \
							void (TObject::*PFunc)(TEvent *)) {

		std::shared_ptr<TEvent> eve_t = std::make_shared<TEvent>();
		if (!(dynamic_cast<XEvent*>(eve_t.get()))) {
			eve_t.reset();
			XEventHandlerException e(XEventHandlerException::E_WRONG_EVENT_TYPE, \
				_T("the member function pointer argument is not inherited from XEvent"));
			throw e;
		}

		evt_handler_caller.Connect(object, PFunc);
		args_container = std::make_shared<CArgumentsOne<TEvent *> >(eve_t.get());
		
		eve = std::move(eve_t);
	}

	XEventHandlerData(const XEventHandlerData &obj) noexcept {

		copy(obj);
	}

	XEventHandlerData(XEventHandlerData &&obj) noexcept = default;

	XEventHandlerData &operator=(const XEventHandlerData &obj) noexcept {

		copy(obj);
		return *this;
	}

	XEventHandlerData &operator=(XEventHandlerData &&obj) noexcept = default;

	inline bool empty() const {

		return !eve || !args_container || evt_handler_caller.empty();
	}

	inline std::shared_ptr<XEvent> getEventObj() { return eve; }
	inline CDelegate getDelegate() { return evt_handler_caller; }
	inline std::shared_ptr<IArguments> getArgsContainer() { return args_container; }

	~XEventHandlerData() {

		args_container.reset();
		evt_handler_caller.reset();
		eve.reset();
	}
};

class XEventHandlerBasic{
protected:
	struct CEvtHandlerRec{
		_plEventId id_event;
		CDelegate evt_handler_caller;
		std::shared_ptr<XEvent> eve;
		std::shared_ptr<IArguments> eve_container;

		CEvtHandlerRec() noexcept : id_event(EVT_NULL), eve(nullptr), \
									eve_container(nullptr){ }

		CEvtHandlerRec(const CEvtHandlerRec &obj) = delete;
		CEvtHandlerRec(CEvtHandlerRec &&obj) noexcept : id_event(obj.id_event), \
								evt_handler_caller(std::move(obj.evt_handler_caller)), \
								eve(std::move(obj.eve)), \
								eve_container(std::move(obj.eve_container)) {

			obj.id_event = EVT_NULL;
		}

		CEvtHandlerRec &operator=(const CEvtHandlerRec &obj) = delete;

		CEvtHandlerRec &operator=(CEvtHandlerRec &&obj) noexcept {

			id_event = obj.id_event;
			evt_handler_caller = std::move(obj.evt_handler_caller);
			eve = std::move(obj.eve);
			eve_container = std::move(obj.eve_container);

			obj.id_event = EVT_NULL;
			return *this;
		}

		inline bool operator==(const CEvtHandlerRec &obj) const {

			return id_event == obj.id_event;
		}

		inline bool operator<(const CEvtHandlerRec &obj) const {

			return id_event < obj.id_event;
		}

		virtual ~CEvtHandlerRec() { }
	};

	struct CEvtHandlerRecEmbedded : public CEvtHandlerRec {
		_plHWND hwnd;
		_plWNDPROC orig_wnd_proc;

		CEvtHandlerRecEmbedded() noexcept : orig_wnd_proc(nullptr) { _plUnsetHWND(hwnd); }

		CEvtHandlerRecEmbedded(const CEvtHandlerRecEmbedded &obj) = delete;

		CEvtHandlerRecEmbedded(CEvtHandlerRecEmbedded &&obj) noexcept : \
								CEvtHandlerRec(std::move(obj)), \
								hwnd(obj.hwnd), orig_wnd_proc(obj.orig_wnd_proc) {

			_plUnsetHWND(obj.hwnd);
			obj.orig_wnd_proc = nullptr;
		}

		CEvtHandlerRecEmbedded &operator=(const CEvtHandlerRecEmbedded &obj) = delete;

		inline CEvtHandlerRecEmbedded &operator=(CEvtHandlerRecEmbedded &&obj) noexcept {

			CEvtHandlerRec::operator=(std::move(obj));
			hwnd = obj.hwnd;
			orig_wnd_proc = obj.orig_wnd_proc;
			_plUnsetHWND(obj.hwnd);
			obj.orig_wnd_proc = nullptr;

			return *this;
		}

		inline bool operator==(const CEvtHandlerRecEmbedded &obj) const noexcept {

			return hwnd == obj.hwnd;
		}

		inline bool operator<(const CEvtHandlerRecEmbedded &obj) const noexcept {

			return hwnd < obj.hwnd;
		}

		virtual ~CEvtHandlerRecEmbedded() { }
	};

	struct CEvtHandlerRecEx : public CEvtHandlerRec{
		_plHWND hwnd;
		int id;
		_plNotificationCode id_ncode;

		CEvtHandlerRecEx() noexcept : CEvtHandlerRec(), id(-1), \
										id_ncode(-1) { _plUnsetHWND(hwnd); }

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

		inline bool operator==(const CEvtHandlerRecEx &obj) const {

			return hwnd == obj.hwnd && id == obj.id && \
				id_event == obj.id_event && id_ncode == obj.id_ncode;
		}

		inline bool operator<(const CEvtHandlerRecEx &obj) const {

			return hwnd < obj.hwnd || \
				(hwnd == obj.hwnd && id < obj.id) || \
				(hwnd == obj.hwnd && id == obj.id && id_event < obj.id_event) || \
				(hwnd == obj.hwnd && id == obj.id && id_event == obj.id_event && \
					id_ncode < obj.id_ncode);
		}

		virtual ~CEvtHandlerRecEx() { }
	};

	typedef std::vector<CEvtHandlerRec>::iterator CEvtHandlerIterator;
	typedef std::vector<CEvtHandlerRec>::const_iterator CEvtHandlerConstIterator;
	typedef std::vector<CEvtHandlerRecEmbedded>::iterator CEvtHandlerIteratorEmb;
	typedef std::vector<CEvtHandlerRecEmbedded>::const_iterator CEvtHandlerConstIteratorEmb;
	typedef std::vector<CEvtHandlerRecEx>::iterator CEvtHandlerIteratorEx;
	typedef std::vector<CEvtHandlerRecEx>::const_iterator CEvtHandlerConstIteratorEx;
};

class XEventHandler;

class XEventHandlerEmbedded : public XEventHandlerBasic{
	static std::vector<CEvtHandlerRecEmbedded> evt_handlers;
	XWindow *window;
	_plHWND hwnd;

	static inline int ConnectImpl(XWindow *window, _plHWND hwnd, \
									const _plEventId id_event, \
									XEventHandlerData evt_handler_data);
	static _plCallbackFnRetValue _plCallbackFnModifier OverrideFunc(_plCallbackFnParams);
public:
	XEventHandlerEmbedded(XWindow *wnd);

	XEventHandlerEmbedded(const XEventHandlerEmbedded &obj) = delete;
	XEventHandlerEmbedded(XEventHandlerEmbedded &&obj) = default;
	XEventHandlerEmbedded &operator=(const XEventHandlerEmbedded &obj) = delete;
	XEventHandlerEmbedded &operator=(XEventHandlerEmbedded &&obj) = default;

	template <class TObject, class TEvent> \
		int Connect(const _plEventId id_event,\
					TObject *obj, \
					void (TObject::*PFunc)(TEvent *));

	inline int Connect(const _plEventId id_event, \
						XEventHandlerData evt_handler_data);

	inline int Connect(_plHWND hwnd, const _plEventId id_event, \
						XEventHandlerData evt_handler_data);

	void DisconnectAll();

	virtual ~XEventHandlerEmbedded();
};

class XEventHandler : public XEventHandlerBasic{
	_plHWND hwnd;
	int id;
	static std::vector<CEvtHandlerRecEx> evt_handlers;

	static int max_id;
	XEventHandlerEmbedded *event_handler_embedded;

protected:
	static _plCallbackFnRetValue _plCallbackFnModifier \
		MainWndProc(_plCallbackFnParams);

	inline int ConnectImpl(const _plEventId id_event, \
							const _plNotificationCode id_ncode, \
							const int id, XEventHandlerData evt_handler_data);

	inline int OverrideWindowEventImpl(const _plEventId id_event, \
										XEventHandlerData evt_handler_data);
	inline int OverrideWindowExtraHwnd(const _plEventId id_event, \
									_plHWND hwnd, XEventHandlerData evt_handler_data);

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

	int Connect(const _plEventId id_event, \
				const _plNotificationCode id_ncode, \
				const int id, XEventHandlerData evt_handler_data);
	int Connect(const _plEventId id_event, \
				const int id, XEventHandlerData evt_handler_data);
	int Connect(const _plEventId id_event, \
				XEventHandlerData evt_handler_data);

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
	virtual int OverrideWindowEvent(const _plEventId id_event, \
									XEventHandlerData evt_handler_data);

	int Disconnect(_plHWND hwnd, const _plEventId id_event, const _plNotificationCode id_ncode, \
					const int id);
	int Disconnect(_plHWND hwnd, const _plEventId id_event, const int id);
	int Disconnect(_plHWND hwnd);
	static void DisconnectAll();
	virtual ~XEventHandler();
};

//****************************************************************************

int XEventHandlerEmbedded::ConnectImpl(XWindow *window, _plHWND hwnd, \
										const _plEventId id_event, \
										XEventHandlerData evt_handler_data) {

	if (id_event == EVT_DESTROY) {
		throw XEventHandlerException(XEventHandlerException::E_WRONG_EVENT_TYPE, \
			_T("the destroy event is restricted to connect. Use the dtor instead"));
	}

	CEvtHandlerRecEmbedded rec;
	CEvtHandlerConstIteratorEmb p;
	rec.hwnd = hwnd;

	p = std::lower_bound(evt_handlers.cbegin(), evt_handlers.cend(), rec);
	if (p != evt_handlers.cend() && *p == rec) {
		throw XEventHandlerException(XEventHandlerException::E_DUPLICATE_HANDLER, \
			_T("only one overrided event per a subclassed window allowed"));
	}

	rec.orig_wnd_proc = _plSetWindowProc(rec.hwnd, OverrideFunc);
	rec.id_event = id_event;

	rec.eve = evt_handler_data.getEventObj();
	assert(rec.eve);
	rec.eve->InitEventData(id_event, 0, window, 0);

	rec.evt_handler_caller = evt_handler_data.getDelegate();
	assert(!rec.evt_handler_caller.empty());

	rec.eve_container = evt_handler_data.getArgsContainer();
	assert(rec.eve_container);

	evt_handlers.emplace_back(std::move(rec));
	std::sort(evt_handlers.begin(), evt_handlers.end());
	return RESULT_SUCCESS;
}

int XEventHandlerEmbedded::Connect(const _plEventId id_event, \
										XEventHandlerData evt_handler_data) {

	return ConnectImpl(window, hwnd, id_event, evt_handler_data);
}

int XEventHandlerEmbedded::Connect(_plHWND hwnd_, const _plEventId id_event, \
										XEventHandlerData evt_handler_data) {

	return ConnectImpl(window, hwnd_, id_event, evt_handler_data);
}

template <class TObject, class TEvent> \
int XEventHandlerEmbedded::Connect(const _plEventId id_event,\
									TObject *obj, \
									void (TObject::*PFunc)(TEvent *)){
	
	return ConnectImpl(window, hwnd, id_event, XEventHandlerData(obj, PFunc));
}

//****************************************************************************

int XEventHandler::ConnectImpl(const _plEventId id_event, \
							const _plNotificationCode id_ncode, \
							const int id, XEventHandlerData evt_handler_data) {

	CEvtHandlerRecEx rec;
	CEvtHandlerIteratorEx p;

	assert(id >= 0);

	XWindow *window = this->GetThisAsXWindow();

	rec.hwnd = this->GetInternalHandle();
	rec.id = (id_event == EVT_CREATE) ? InitializeId() : id;
	rec.id_event = id_event;
	rec.id_ncode = id_ncode;

	p = std::lower_bound(evt_handlers.begin(), evt_handlers.end(), rec);
	if (p != evt_handlers.end() && *p == rec)
		return XEventHandlerException::W_DUPLICATE_HANDLER;

	rec.eve = evt_handler_data.getEventObj();
	assert(rec.eve);
	rec.eve->InitEventData(id_event, id_ncode, window, rec.id);

	rec.evt_handler_caller = evt_handler_data.getDelegate();
	assert(!rec.evt_handler_caller.empty());

	rec.eve_container = evt_handler_data.getArgsContainer();
	assert(rec.eve_container);

	//evt_handlers.insert(p, std::move(rec));
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

	return ConnectImpl(id_event, id_ncode, id, XEventHandlerData(obj, PFunc));
}

template <class TObject, class TEvent> \
		int XEventHandler::Connect(const _plEventId id_event,\
								const int id,\
								TObject *obj, \
								void (TObject::*PFunc)(TEvent *)){

	return ConnectImpl(id_event, 0, id, XEventHandlerData(obj, PFunc));
}

template <class TObject, class TEvent> \
		int XEventHandler::Connect(const _plEventId id_event,\
								TObject *obj, \
								void (TObject::*PFunc)(TEvent *)){

	return ConnectImpl(id_event, 0, 0, XEventHandlerData(obj, PFunc));
}

int XEventHandler::OverrideWindowEventImpl(const _plEventId id_event, \
											XEventHandlerData evt_handler_data) {

	if (!event_handler_embedded)
		event_handler_embedded = new XEventHandlerEmbedded(GetThisAsXWindow());
	return event_handler_embedded->Connect(id_event, std::move(evt_handler_data));
}

int XEventHandler::OverrideWindowExtraHwnd(const _plEventId id_event, \
										_plHWND hwnd, XEventHandlerData evt_handler_data) {

	assert(event_handler_embedded);
	return event_handler_embedded->Connect(hwnd, id_event, std::move(evt_handler_data));
}

template <class TObject, class TEvent> \
int XEventHandler::OverrideWindowEvent(const _plEventId id_event,\
										TObject *obj, \
										void (TObject::*PFunc)(TEvent *)){

	return OverrideWindowEvent(id_event, XEventHandlerData(obj, PFunc));
}
