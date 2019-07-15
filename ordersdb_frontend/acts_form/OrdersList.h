#pragma once
#include <memory>
#include <xwindows/XWidget.h>
#include <xwindows/XDynamicSizer.h>

struct IDbConnection;
class CDbGrid;
class CDbTable;

class COrdersList {
	std::shared_ptr<CDbTable> db_table;
	CDbGrid *grid;
	XWidget *grid_as_window;
	XWidget *db_navigator;
	int db_navigator_height;
	float multiplier;

	XDynamicSizer grid_sizer, nav_sizer, *prev_sizer;

	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn);
public:
	COrdersList(const int margins_, const float multiplier_, const int db_navigator_height);

	void initDbTable(std::shared_ptr<IDbConnection> conn_);
	inline void initSizers(XPoint initial_coords, XSize parent_size, \
							XDynamicSizer *prev_sizer);

	inline XDynamicSizer &getFirstSizer();
	inline const XDynamicSizer &getLastSizer() const;

	void displayWidgets(XWindow *parent);
	inline void updateSizers();
	inline void resize();

	virtual ~COrdersList();
};

//*****************************************************

void COrdersList::initSizers(XPoint initial_coords, XSize size, \
								XDynamicSizer *prev_sizer) {

	assert(prev_sizer);
	this->prev_sizer = prev_sizer;

	grid_sizer.addToHorzSizersChain(*prev_sizer);
	grid_sizer.setHeightMultiplier(0.5F);

	nav_sizer.addToVertSizersChain(grid_sizer);
	nav_sizer.setRelativeHeight(db_navigator_height);
	nav_sizer.freezeHeight();
}

void COrdersList::updateSizers() {

	grid_sizer.updateHorzSizersChain(*prev_sizer);
	nav_sizer.updateVertSizersChain(grid_sizer);
}

void COrdersList::resize() {

	grid_sizer.moveWidget(grid_as_window);
	nav_sizer.moveWidget(db_navigator);
}

XDynamicSizer &COrdersList::getFirstSizer() {

	return grid_sizer;
}

const XDynamicSizer &COrdersList::getLastSizer() const {

	return nav_sizer;
}