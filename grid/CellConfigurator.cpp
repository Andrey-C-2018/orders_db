#include "TextCell.h"
#include "GridLine.h"
#include "CellConfigurator.h"

void CCellConfigurator::Configure(CGridLine &line) {

	grid_line_size = line.GetGridLineSize();
	assert(grid_line_size >= 0);
	line.SetBackground(background_brush, background_pen);
}

void CCellConfigurator::Configure(CTextCell &text_cell) {

	SetCellHeightAndMargins(text_cell.EvalCellHeightByTextHeight(text_height), \
							text_cell.GetMarginsWidth());
}

void CCellConfigurator::Configure(CDispatcherCell &disp_cell) { }
