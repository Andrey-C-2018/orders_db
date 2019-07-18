#include "XComboBoxEx.h"

XComboKeyPressEvtHandler::XComboKeyPressEvtHandler() : \
								orig_wnd_proc_combo(nullptr), \
								orig_wnd_proc_item(nullptr), \
								orig_wnd_proc_list(nullptr) { }

XComboKeyPressEvtHandler::~XComboKeyPressEvtHandler() { }

//*****************************************************

std::unique_ptr<XComboKeyPressEvtHandler> XComboBoxEx::keypress_handler;

XComboBoxEx::XComboBoxEx() { }

XComboBoxEx::XComboBoxEx(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height) : \
							XComboBox(parent, flags, label, x, y, width, height) { }

_plCallbackFnRetValue _plCallbackFnModifier \
	XComboBoxEx::OverrideComboFunc(_plCallbackFnParams) {

	return keypress_handler->ExecComboFunc(_plCallbackFnParamsList);
}

_plCallbackFnRetValue _plCallbackFnModifier \
	XComboBoxEx::OverrideItemFunc(_plCallbackFnParams) {

	return keypress_handler->ExecItemFunc(_plCallbackFnParamsList);
}

_plCallbackFnRetValue _plCallbackFnModifier \
	XComboBoxEx::OverrideListFunc(_plCallbackFnParams) {

	return keypress_handler->ExecListFunc(_plCallbackFnParamsList);
}

XComboBoxEx::~XComboBoxEx() { }
