#pragma once
#include <basic/tstring.h>
#include <basic/ImmutableString.h>

class XWindow;
class XEventHandlerData;

struct IGridCellWidget {
	virtual void CreateCellWidget(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height) = 0;
	
	virtual void SetOnChangeHandler(XEventHandlerData on_change) = 0;
	virtual void SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) = 0;
	virtual void SetOnKeyPressHandler(XEventHandlerData on_key_press) = 0;

	virtual void SetCurrentField(const size_t field) = 0;

	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void MoveWindow(const int x, const int y, \
							const int width, const int height) = 0;
	virtual void SetFocus() = 0;

	virtual ImmutableString<Tchar> GetLabel() = 0;
	virtual void SetLabel(ImmutableString<Tchar> label) = 0;

	virtual int GetId() const = 0;

	virtual ~IGridCellWidget() { }
};