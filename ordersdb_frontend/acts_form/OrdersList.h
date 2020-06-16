#pragma once
#include <memory>
#include <xwindows/XWidget.h>
#include <xwindows_ex/XDynamicSizer.h>

struct IDbConnection;
struct IDbTableEventsHandler;
struct CPaymentsConstraints;
class CDbGrid;
class CDbTable;

class COrdersList {
	std::shared_ptr<CDbTable> db_table;
	CDbGrid *grid;
	XWidget *grid_as_window;
	XWidget *db_navigator;
	int db_navigator_height;
	float multiplier;

	bool cr_grid, cr_navigator;

	XDynamicSizer grid_sizer, nav_sizer, *prev_sizer;

	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn, \
											const int def_adv_id);
	void createCellWidgetsAndAttachToGrid(CDbGrid *grid);
public:
	COrdersList(const int margins_, const float multiplier_, const int db_navigator_height);

	COrdersList(const COrdersList &obj) = delete;
	COrdersList(COrdersList &&obj) = delete;
	COrdersList &operator=(const COrdersList &obj) = delete;
	COrdersList &operator=(COrdersList &&obj) = delete;

	void initDbTable(std::shared_ptr<IDbConnection> conn_, const int def_adv_id, \
						std::shared_ptr<CPaymentsConstraints> constraints);
	void initDbTableEvtHandler(std::shared_ptr<IDbTableEventsHandler> evt_handler);
	inline void initSizers(XPoint initial_coords, XSize parent_size, \
							XDynamicSizer *prev_sizer);

	inline XDynamicSizer &getFirstSizer();
	inline const XDynamicSizer &getLastSizer() const;
	inline std::shared_ptr<CDbTable> getDbTable();

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
	grid_sizer.setHeightMultiplier(0.4F);

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

std::shared_ptr<CDbTable> COrdersList::getDbTable() {

	return db_table;
}