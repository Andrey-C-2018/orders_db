#pragma once

class CDelegate;

struct IGridCellWidget {
	virtual void SetOnChangeHandler(CDelegate &on_change_func_delegate) = 0;
	virtual ~IGridCellWidget() { }
};