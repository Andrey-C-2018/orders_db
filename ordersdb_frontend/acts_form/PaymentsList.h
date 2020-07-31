#pragma once
#include <memory>
#include <date/Date.h>
#include <xwindows/XWidget.h>
#include <xwindows_ex/XDynamicSizer.h>
#include "PaymentsNavPanel.h"

struct IDbConnection;
class CDbGrid;
class CDbTable;
class CDbComboBoxCellWidget;
struct CPaymentsConstraints;
class CPaymentsGridEvtHandler;

class CPaymentsList {
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<CDbTable> db_table;
	std::shared_ptr<CPaymentsGridEvtHandler> grid_evt_handler;
	CDbGrid *grid;
	XWidget *grid_as_window;
	XWidget *db_navigator;
	CPaymentsNavPanel *panel;

	bool cr_grid, cr_navigator, cr_panel;

	enum {
		DEF_PANEL_HEIGHT = 120
	};
	int db_navigator_height;
	float multiplier;

	XDynamicSizer grid_sizer, nav_sizer, panel_sizer;
	const XDynamicSizer *prev_sizer;

	CDbComboBoxCellWidget *stages_list, *informers_list, *checkers_list;

	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn, \
											const int def_center, const int def_order, \
											CDate def_order_date);
	void createCellWidgetsAndAttachToGrid(CDbGrid *grid);
public:
	CPaymentsList(const int margins_, const int db_navigator_height);

	CPaymentsList(const CPaymentsList &obj) = delete;
	CPaymentsList(CPaymentsList &&obj) = delete;
	CPaymentsList &operator=(const CPaymentsList &obj) = delete;
	CPaymentsList &operator=(CPaymentsList &&obj) = delete;

	void initDbTable(std::shared_ptr<IDbConnection> conn_, const int def_center, \
						const int def_order, CDate def_order_date, \
						std::shared_ptr<CPaymentsConstraints> constraints);
	void initDbTableEvtHandler(std::shared_ptr<IDbTableEventsHandler> evt_handler);
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
	grid_sizer.setMinusY(db_navigator_height + DEF_PANEL_HEIGHT);

	nav_sizer.addToVertSizersChain(grid_sizer);
	nav_sizer.setRelativeHeight(db_navigator_height);
	nav_sizer.freezeHeight();

	panel_sizer.addToVertSizersChain(nav_sizer);
	nav_sizer.freezeHeight();
}

void CPaymentsList::updateSizers() {

	grid_sizer.updateVertSizersChain(*prev_sizer);
	nav_sizer.updateVertSizersChain(grid_sizer);
	panel_sizer.updateVertSizersChain(nav_sizer);
}

void CPaymentsList::resize() {

	grid_sizer.moveWidget(grid_as_window);
	nav_sizer.moveWidget(db_navigator);
	panel_sizer.moveWidget(panel);
}

XDynamicSizer &CPaymentsList::getFirstSizer() {

	return grid_sizer;
}

const XDynamicSizer &CPaymentsList::getLastSizer() const {

	return panel_sizer;
}

std::shared_ptr<CDbTable> CPaymentsList::getDbTable() {

	return db_table;
}