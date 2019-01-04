#include "TextCell.h"
#include "DispatcherCell.h"
#include "GridLine.h"
#include "CellConfigurator.h"

void CCellConfigurator::Configure(CGridLine &line) {

	grid_line_size = line.GetGridLineSize();
	assert(grid_line_size >= 0);
	line.SetBackground(background_brush, background_pen);
}

void CCellConfigurator::Configure(CTextCell &text_cell) {

	assert(text_height != -1);
	assert(grid_line_size != -1);

	cell_height = text_cell.EvalCellHeightByTextHeight(text_height);
	cell_margins_width = text_cell.GetMarginsWidth();
}

void CCellConfigurator::Configure(CDispatcherCell &disp_cell) {

	assert(text_height != -1);
	assert(grid_line_size != -1);

	cell_height = disp_cell.EvalCellHeightByTextHeight(text_height);
	cell_margins_width = disp_cell.GetMarginsWidth();
}
