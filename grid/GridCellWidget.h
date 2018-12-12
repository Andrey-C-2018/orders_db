#pragma once

class XEventHandlerData;

class CGridCellWidget {

	CGridCellWidget(const CGridCellWidget &obj) = delete;
	CGridCellWidget &operator=(const CGridCellWidget &obj) = delete;
public:
	CGridCellWidget();
	CGridCellWidget(CGridCellWidget &&obj) = default;
	CGridCellWidget &operator=(CGridCellWidget &&obj) = default;

	virtual void Create(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height)

	virtual void SetOnChangeHandler(XEventHandlerData on_change_func_delegate) = 0;

	inline void SetFocus();
	inline void Hide();
	inline void Show();

	inline ImmutableString<Tchar> GetLabel() const;
	inline void SetLabel(ImmutableString<Tchar> label);

	virtual void MoveWindow(const int x, const int y, \
							const int width, const int height);

	inline int GetId() const;

	virtual ~CGridCellWidget() { }
};