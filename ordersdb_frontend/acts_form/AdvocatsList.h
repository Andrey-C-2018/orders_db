#pragma once
#include <memory>
#include <xwindows/XWidget.h>
#include <xwindows_ex/XDynamicSizer.h>

struct IDbConnection;
struct IDbTableEventsHandler;
class CDbGrid;
class CDbTable;

class CAdvocatsList {
	std::shared_ptr<CDbTable> db_table;
	CDbGrid *grid;
	XWidget *grid_as_window;
	XWidget *db_navigator;
	int width, db_navigator_height;

	XDynamicSizer grid_sizer, nav_sizer;
	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn);
public:
	CAdvocatsList(const int margins_, const int width_, const int db_navigator_height_);

	void initDbTable(std::shared_ptr<IDbConnection> conn_);
	void initDbTableEvtHandler(std::shared_ptr<IDbTableEventsHandler> evt_handler);
	inline void initSizers(XPoint initial_coords, XSize parent_size);

	inline XDynamicSizer &getFirstSizer();
	inline const XDynamicSizer &getLastSizer() const;
	inline std::shared_ptr<const CDbTable> getDbTable() const;

	void displayWidgets(XWindow *parent);

	inline bool updateSizers(XSize new_size);
	inline void resize();

	virtual ~CAdvocatsList();
};

//*****************************************************

void CAdvocatsList::initSizers(XPoint initial_coords, XSize size) {

	grid_sizer.setCoords(initial_coords);
	grid_sizer.setParentSize(size);
	grid_sizer.setRelativeWidth(width);
	grid_sizer.freezeWidth();
	grid_sizer.setMinusY(db_navigator_height);

	nav_sizer.addToVertSizersChain(grid_sizer);
}

bool CAdvocatsList::updateSizers(XSize new_size) {

	bool ret_value = grid_sizer.setParentSize(new_size);
	nav_sizer.updateVertSizersChain(grid_sizer);

	return ret_value;
}

void CAdvocatsList::resize() {

	grid_sizer.moveWidget(grid_as_window);
	nav_sizer.moveWidget(db_navigator);
}

XDynamicSizer &CAdvocatsList::getFirstSizer() {

	return grid_sizer;
}

const XDynamicSizer &CAdvocatsList::getLastSizer() const {

	return nav_sizer;
}

std::shared_ptr<const CDbTable> CAdvocatsList::getDbTable() const {

	return db_table;
}