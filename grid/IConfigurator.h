#pragma once

class CGridLine;
class CTextCell;
class CEditableTextCell;

struct IConfigurator {
	virtual void Configure(CGridLine &line) = 0;
	virtual void Configure(CTextCell &text_cell) = 0;
	virtual void Configure(CEditableTextCell &editable_cell) = 0;
	virtual ~IConfigurator() { }
};
