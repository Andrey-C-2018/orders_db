#pragma once

class CGridLine;
class CTextCell;
class CDispatcherCell;

struct IConfigurator {
	virtual void Configure(CGridLine &line) = 0;
	virtual void Configure(CTextCell &text_cell) = 0;
	virtual void Configure(CDispatcherCell &disp_cell) = 0;
	virtual ~IConfigurator() { }
};
