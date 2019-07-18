#pragma once
#include "XComboBox.h"

class XComboKeyPressEvtHandler final {
	struct EvtHandlerRec {
		_plHWND hwnd;
	};
	//static std::vector<>

	CDelegate keypress_handler_caller;
	std::shared_ptr<XEvent> keypress_eve;
	std::shared_ptr<IArguments> keypress_eve_arg;

	_plWNDPROC orig_wnd_proc_combo;
	_plWNDPROC orig_wnd_proc_item;
	_plWNDPROC orig_wnd_proc_list;

	inline _plCallbackFnRetValue _plCallbackFnModifier \
		WndProc(_plCallbackFnParams, _plWNDPROC orig_wnd_proc);
public:
	XComboKeyPressEvtHandler();

	inline _plCallbackFnRetValue _plCallbackFnModifier ExecComboFunc(_plCallbackFnParams);
	inline _plCallbackFnRetValue _plCallbackFnModifier ExecItemFunc(_plCallbackFnParams);
	inline _plCallbackFnRetValue _plCallbackFnModifier ExecListFunc(_plCallbackFnParams);

	~XComboKeyPressEvtHandler();
};

class XComboBoxEx : public XComboBox {
	static std::unique_ptr<XComboKeyPressEvtHandler> keypress_handler;

	static _plCallbackFnRetValue _plCallbackFnModifier OverrideComboFunc(_plCallbackFnParams);
	static _plCallbackFnRetValue _plCallbackFnModifier OverrideItemFunc(_plCallbackFnParams);
	static _plCallbackFnRetValue _plCallbackFnModifier OverrideListFunc(_plCallbackFnParams);
public:
	XComboBoxEx();
	XComboBoxEx(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height);

	XComboBoxEx(const XComboBoxEx &obj) = delete;
	XComboBoxEx(XComboBoxEx &&obj) = default;
	XComboBoxEx &operator=(const XComboBoxEx &obj) = delete;
	XComboBoxEx &operator=(XComboBoxEx &&obj) = default;

	void OverrideKeyPressEvent(XEventHandlerData evt_handler_data);

	virtual ~XComboBoxEx();
};

//*****************************************************

_plCallbackFnRetValue _plCallbackFnModifier \
	XComboKeyPressEvtHandler::WndProc(_plCallbackFnParams, _plWNDPROC orig_wnd_proc) {

	_plEventId id_event = _plGetEventId(_plCallbackFnParamsList);
	if (id_event != EVT_KEYDOWN)
		return _plDefaultEventAction(orig_wnd_proc, _plCallbackFnParamsList);

	keypress_eve->PostInit(_plCallbackFnParamsList);

	keypress_handler_caller.Call(keypress_eve_arg.get());
	if (keypress_eve->GetDefaultActionStatus()) {
		keypress_eve->ExecuteDefaultEventAction(false);
		return _plDefaultEventAction(orig_wnd_proc, _plCallbackFnParamsList);
	}

	return EVT_DONT_PROCESS;
}

_plCallbackFnRetValue _plCallbackFnModifier \
	XComboKeyPressEvtHandler::ExecComboFunc(_plCallbackFnParams) {

	return WndProc(_plCallbackFnParamsList, orig_wnd_proc_combo);
}

_plCallbackFnRetValue _plCallbackFnModifier \
	XComboKeyPressEvtHandler::ExecItemFunc(_plCallbackFnParams) {

	return WndProc(_plCallbackFnParamsList, orig_wnd_proc_item);
}

_plCallbackFnRetValue _plCallbackFnModifier \
	XComboKeyPressEvtHandler::ExecListFunc(_plCallbackFnParams) {

	return WndProc(_plCallbackFnParamsList, orig_wnd_proc_list);
}