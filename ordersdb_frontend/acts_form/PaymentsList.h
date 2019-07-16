#pragma once
#include <memory>
#include <date/Date.h>
#include <xwindows/XWidget.h>
#include <xwindows/XDynamicSizer.h>

struct IDbConnection;
class CDbGrid;
class CDbTable;

class CPaymentsList {
	std::shared_ptr<CDbTable> db_table;
	CDbGrid *grid;
	XWidget *grid_as_window;
	XWidget *db_navigator;
	int db_navigator_height;
	float multiplier;

	XDynamicSizer grid_sizer, nav_sizer;
	const XDynamicSizer *prev_sizer;

	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn, \
											const int def_center, const int def_order, \
											CDate def_order_date);
public:
	CPaymentsList(const int margins_, const int db_navigator_height);

	void initDbTable(std::shared_ptr<IDbConnection> conn_, const int def_center, \
						const int def_order, CDate def_order_date);
	inline void initSizers(XPoint initial_coords, XSize parent_size, \
							const XDynamicSizer *prev_sizer);

	inline XDynamicSizer &getFirstSizer();
	inline const XDynamicSizer &getLastSizer() const;
	inline std::shared_ptr<CDbTable> getDbTable();

	void displayWidgets(XWindow *parent);
	inline void updateSizers();
	inline void resize();

	virtual ~CPaymentsList();
};

//*****************************************************

void CPaymentsList::initSizers(XPoint initial_coords, XSize size, \
								const XDynamicSizer *prev_sizer) {

	assert(prev_sizer);
	this->prev_sizer = prev_sizer;

	grid_sizer.addToVertSizersChain(*prev_sizer);
	grid_sizer.setMinusY(db_navigator_height);

	nav_sizer.addToVertSizersChain(grid_sizer);
	nav_sizer.freezeHeight();
}

void CPaymentsList::updateSizers() {

	grid_sizer.updateVertSizersChain(*prev_sizer);
	nav_sizer.updateVertSizersChain(grid_sizer);
}

void CPaymentsList::resize() {

	grid_sizer.moveWidget(grid_as_window);
	nav_sizer.moveWidget(db_navigator);
}

XDynamicSizer &CPaymentsList::getFirstSizer() {

	return grid_sizer;
}

const XDynamicSizer &CPaymentsList::getLastSizer() const {

	return nav_sizer;
}

std::shared_ptr<CDbTable> CPaymentsList::getDbTable() {

	return db_table;
}