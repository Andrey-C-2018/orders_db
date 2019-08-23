#include <functional>
#include <assert.h>
#include "XEventHandler.h"
#include "XWindow.h"

XEventHandlerException::XEventHandlerException(const int err_code, const Tchar *err_descr) : \
				XException(err_code, err_descr){ }

XEventHandlerException::XEventHandlerException(const XEventHandlerException &obj) : \
				XException(obj){ }

XEventHandlerException::~XEventHandlerException(){ }

//****************************************************************************

std::vector<XEventHandlerEmbedded::CEvtHandlerRecEmbedded> XEventHandlerEmbedded::evt_handlers;

XEventHandlerEmbedded::XEventHandlerEmbedded(XWindow *wnd) : window(wnd) {

	assert(window);
	hwnd = window->GetInternalHandle();
	if(!window->IsCreated()){
		XEventHandlerException e(XEventHandlerException::E_WINDOW_NOT_EXIST, \
		_T("The window is not yet created, so its events cannot be overridden. Class name = "));
		e<< window->GetClassName();
		throw e;
	}
}

_plCallbackFnRetValue _plCallbackFnModifier \
	XEventHandlerEmbedded::OverrideFunc(_plCallbackFnParams) {

	CEvtHandlerRecEmbedded rec;
	CEvtHandlerIteratorEmb p;

	rec.hwnd = _plGetWindowHandle(_plCallbackFnParamsList);
	rec.id_event = _plGetEventId(_plCallbackFnParamsList);

	p = std::lower_bound(evt_handlers.begin(), evt_handlers.end(), rec);
	assert(p != evt_handlers.end() && *p == rec);
	
	if(p->id_event != rec.id_event)
		return _plDefaultEventAction(p->orig_wnd_proc, _plCallbackFnParamsList);

	p->eve->PostInit(_plCallbackFnParamsList);

	XEvent *eve = p->eve.get();
	_plWNDPROC orig_wnd_proc = p->orig_wnd_proc;

	p->evt_handler_caller.Call(p->eve_container.get());
	if (eve->GetDefaultActionStatus()) {
		eve->ExecuteDefaultEventAction(false);
		return _plDefaultEventAction(orig_wnd_proc, _plCallbackFnParamsList);
	}

	return EVT_DONT_PROCESS;
}

void XEventHandlerEmbedded::DisconnectAll(){

	CEvtHandlerRecEmbedded rec;
	CEvtHandlerIteratorEmb p;

	rec.hwnd = window->GetInternalHandle();
	p = std::lower_bound(evt_handlers.begin(), evt_handlers.end(), rec);
	if (!(p != evt_handlers.end() && *p == rec))
		return;

	assert(p->eve_container);
	assert(p->eve);
	assert(p->orig_wnd_proc);

	p->eve_container.reset();
	p->evt_handler_caller.reset();
	p->eve.reset();

	p->id_event = EVT_NULL;
}

XEventHandlerEmbedded::~XEventHandlerEmbedded(){

	DisconnectAll();
}

//****************************************************************************

std::vector<XEventHandler::CEvtHandlerRecEx> XEventHandler::evt_handlers;
int XEventHandler::max_id = 1;

XEventHandler::XEventHandler() : event_handler_embedded(nullptr), id(-1){
	
	_plUnsetHWND(hwnd);
}

XEventHandler::XEventHandler(XEventHandler &&obj) : \
							event_handler_embedded(obj.event_handler_embedded), \
							id(obj.id){

	obj.event_handler_embedded = nullptr;
	hwnd = obj.hwnd;
	_plUnsetHWND(obj.hwnd);
	obj.id = -1;
}

XEventHandler &XEventHandler::operator=(XEventHandler &&obj) {

	event_handler_embedded = obj.event_handler_embedded;
	obj.event_handler_embedded = nullptr;
	hwnd = obj.hwnd;
	_plUnsetHWND(obj.hwnd);
	id = obj.id;
	obj.id = -1;
	return *this;
}

_plCallbackFnRetValue _plCallbackFnModifier \
XEventHandler::MainWndProc(_plCallbackFnParams){
	CEvtHandlerRecEx rec;
	CEvtHandlerIteratorEx p;
	_plHWND hwnd_backup;

	rec.id = _plGetWindowId(_plCallbackFnParamsList);
	rec.id_event = _plGetEventId(_plCallbackFnParamsList);
	rec.id_ncode = _plGetNotificationCode(_plCallbackFnParamsList);
	hwnd_backup = _plGetWindowHandle(_plCallbackFnParamsList);
	rec.hwnd = hwnd_backup;

	if(rec.id_event == EVT_CREATE){
		XEventHandler *wnd = _plCreateEventExtractWindowObject(_plCallbackFnParamsList);
		assert(wnd);
		wnd->hwnd = rec.hwnd;
		wnd->OnWindowCreationCompleted();

		_plUnsetHWND(rec.hwnd);
		rec.id = wnd->GetId();
	} 

	p = std::lower_bound(evt_handlers.begin(), evt_handlers.end(), rec);

	if (!(p != evt_handlers.end() && *p == rec)) {
		return _plDefaultEventAction(_plCallbackFnParamsList);
	}

	p->hwnd = hwnd_backup;

	p->eve->PostInit(_plCallbackFnParamsList);
	bool def_action = p->eve->GetDefaultActionStatus(); // !!!

	try {
		p->evt_handler_caller.Call(p->eve_container.get());
	}
	catch (XException &e) {
		ErrorBox(e.what());
		PostQuitMessage(RESULT_FAIL);
	}
	catch (std::exception &e) {
		ErrorBoxANSI(e.what());
		PostQuitMessage(RESULT_FAIL);
	}
	catch (...) {
		ErrorBox(_T("undefined exception"));
		PostQuitMessage(RESULT_FAIL);
	}

	if(def_action)
		return _plDefaultEventAction(_plCallbackFnParamsList);

	return EVT_DONT_PROCESS;
}

int XEventHandler::Connect(const _plEventId id_event, \
							const _plNotificationCode id_ncode, \
							const int id, XEventHandlerData evt_handler_data) {

	return ConnectImpl(id_event, id_ncode, id, std::move(evt_handler_data));
}

int XEventHandler::Connect(const _plEventId id_event, \
							const int id, XEventHandlerData evt_handler_data) {

	return ConnectImpl(id_event, 0, id, std::move(evt_handler_data));
}

int XEventHandler::Connect(const _plEventId id_event, \
							XEventHandlerData evt_handler_data) {

	return ConnectImpl(id_event, 0, 0, std::move(evt_handler_data));
}

int XEventHandler::OverrideWindowEvent(const _plEventId id_event, \
										XEventHandlerData evt_handler_data) {

	return OverrideWindowEventImpl(id_event, std::move(evt_handler_data));
}

int XEventHandler::Disconnect(_plHWND hwnd,\
							  const _plEventId id_event,\
							  const _plNotificationCode id_ncode,\
							  const int id){
	CEvtHandlerRecEx rec;
	CEvtHandlerIteratorEx p;

	rec.hwnd = hwnd;
	rec.id = id;
	rec.id_event = id_event;
	rec.id_ncode = id_ncode;
	p = std::lower_bound(evt_handlers.begin(), evt_handlers.end(), rec);
	if(!(p != evt_handlers.cend() && *p == rec))
		return XEventHandlerException::W_HANDLER_NOT_FOUND;

	p->eve_container.reset();
	p->evt_handler_caller.reset();
	p->eve.reset();
	evt_handlers.erase(p);
	return RESULT_SUCCESS;
}

int XEventHandler::Disconnect(_plHWND hwnd,\
							  const _plEventId id_event,\
							  const int id){
	CEvtHandlerRecEx rec;
	std::pair<CEvtHandlerIteratorEx, CEvtHandlerIteratorEx> p;

	rec.hwnd = hwnd;
	rec.id = id;
	rec.id_event = id_event;
	p = std::equal_range(evt_handlers.begin(), evt_handlers.end(), rec, \
							[](const CEvtHandlerRecEx &l, const CEvtHandlerRecEx &r) -> bool{

								return l.hwnd < r.hwnd || \
									(l.hwnd == r.hwnd && l.id < r.id) || \
									(l.hwnd == r.hwnd && l.id == r.id && \
										l.id_event < r.id_event);
							});
	if(p.first == p.second)
		return XEventHandlerException::W_HANDLER_NOT_FOUND;

	std::for_each(p.first, p.second, \
					[](CEvtHandlerRecEx &rec) {
						assert(rec.eve_container);
						assert(rec.eve);
						
						rec.eve_container.reset();
						rec.evt_handler_caller.reset();
						rec.eve.reset();
					});
	evt_handlers.erase(p.first, p.second);
	return RESULT_SUCCESS;
}

int XEventHandler::Disconnect(_plHWND hwnd){
	CEvtHandlerRecEx rec;
	std::pair<CEvtHandlerIteratorEx, CEvtHandlerIteratorEx> p;

	rec.hwnd = hwnd;
	p = std::equal_range(evt_handlers.begin(), evt_handlers.end(), rec, \
						[](const CEvtHandlerRecEx &l, const CEvtHandlerRecEx &r) -> bool{

							return l.hwnd < r.hwnd;
						});
	if(p.first == p.second)
		return XEventHandlerException::W_HANDLER_NOT_FOUND;

	size_t debug_dist = 0;
	assert((debug_dist = std::distance(p.first, p.second)) >= 0);

	std::for_each(p.first, p.second,\
					[](CEvtHandlerRecEx &rec) {
						assert(rec.eve_container);
						assert(rec.eve);

						rec.eve_container.reset();
						rec.evt_handler_caller.reset();
						rec.eve.reset();
					});
	evt_handlers.erase(p.first, p.second);
	return RESULT_SUCCESS;
}

void XEventHandler::DisconnectAll(){

	std::for_each(evt_handlers.begin(), evt_handlers.end(),\
					[](CEvtHandlerRecEx &rec) {
						assert(rec.eve_container);
						assert(rec.eve);

						rec.eve_container.reset();
						rec.evt_handler_caller.reset();
						rec.eve.reset();
					});
	evt_handlers.clear();
	evt_handlers.shrink_to_fit();
}

XEventHandler::~XEventHandler(){

	if(_plIsHWNDSet(hwnd)) 
		Disconnect(hwnd);
	if(event_handler_embedded) 
		delete event_handler_embedded;
}
