#pragma once
#include <memory>
#include <basic/XConv.h>
#include <xwindows/XEdit.h>

class CDbTable;
class CDbTableEventsHandler;

class CDbNavigator : public XEdit {
	size_t curr_record, records_count;
	Tchar buffer[getDigitsCountOfType<size_t>() * 2 + 5];
	std::shared_ptr<CDbTable> db_table;
	std::shared_ptr<CDbTableEventsHandler> evt_handler;

	inline const Tchar *ConvToStr();
public:
	CDbNavigator(std::shared_ptr<CDbTable> db_table);

	void Create(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height) override;
	inline void Refresh();

	virtual ~CDbNavigator();
};

const Tchar *CDbNavigator::ConvToStr() {

	size_t size;
	XConv::ToString(curr_record, buffer, size);
	Tstrcpy(&buffer[size], _T(" / "));
	XConv::ToString(records_count, &buffer[size + 3], size);

	return buffer;
}

void CDbNavigator::Refresh() {

	ConvToStr();
	SetLabel(buffer);
}