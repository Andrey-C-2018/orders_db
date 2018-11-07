#pragma once

class CEditableGrid;
class XCommandEvent;
class XKeyboardEvent;

struct ICellWidget {
	virtual void SetOnChangeHandler(void (CEditableGrid::*PFunc)(XCommandEvent *eve)) = 0;
	virtual void SetOnLooseFocusHandler(void (CEditableGrid::*PFunc)(XCommandEvent *eve)) = 0;
	virtual void SetOnKeyPressHandler(void (CEditableGrid::*PFunc)(XKeyboardEvent *eve)) = 0;
	virtual void Move(int x, int y, int width, int height) = 0;

	virtual ~ICellWidget(){ }
};