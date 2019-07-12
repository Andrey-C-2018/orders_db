#pragma once
#include <memory>
#include <xwindows/XWidget.h>
#include <xwindows/XDynamicSizer.h>

struct IDbConnection;
class CDbGrid;
class CDbTable;

class CAdvocatsListManager {
	std::shared_ptr<CDbTable> db_table;
	CDbGrid *grid;
	XWidget *grid_as_window;
	XWidget *db_navigator;

	XDynamicSizer grid_sizer, nav_sizer;

	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn);

public:
	CAdvocatsListManager();

	void Init(std::shared_ptr<IDbConnection> conn_, XWindow *parent);
	void CreateWidgets(XWindow *parent, const int flags, \
								const Tchar *label, \
								const int x, const int y, \
								const int width, const int height);
	inline void resize(XRect r);

	virtual ~CAdvocatsListManager();
};

//*****************************************************

void CAdvocatsListManager::resize(XRect r) {

	grid_sizer.setCoords(XPoint(r.left, r.top));
	r = grid_sizer.resize(XSize(r.right - r.left, r.bottom - r.top));

	grid_as_window->MoveWindow(r.left, r.top, r.right - r.left, \
								r.bottom - r.top);

	r = nav_sizer.resize(&grid_sizer);
	db_navigator->MoveWindow(r.left, r.top, r.right - r.left, \
								r.bottom - r.top);
}
