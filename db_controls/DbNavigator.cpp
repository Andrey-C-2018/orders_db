#include <db_ext/IDbTableEventsHandler.h>
#include <db_ext/DbTable.h>
#include "DbNavigator.h"

class CDbTableEventsHandler : public IDbTableEventsHandler {
	CDbNavigator &db_navigator;
	size_t &curr_record, &records_count;
public:
	CDbTableEventsHandler(CDbNavigator &db_navigator_, \
							size_t &curr_record_, size_t &records_count_) : \
								db_navigator(db_navigator_), \
								curr_record(curr_record_), \
								records_count(records_count_) { }

	void OnCurrRecordNoChanged(const size_t new_record) override {

		curr_record = new_record;
		db_navigator.Refresh();
	}

	void OnFieldsCountChanged(const size_t new_fields_count) override { }

	void OnRecordsCountChanged(const size_t new_records_count) override {
	
		records_count = new_records_count;
		db_navigator.Refresh();
	}

	void OnTableReset() override { }

	virtual ~CDbTableEventsHandler() { }
};

//*****************************************************

CDbNavigator::CDbNavigator(std::shared_ptr<CDbTable> db_table_) : \
				db_table(db_table_), \
				evt_handler(std::make_shared<CDbTableEventsHandler>(\
										*this, curr_record, records_count)){

	db_table->ConnectDbEventsHandler(evt_handler);
}

void CDbNavigator::Create(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) {

	assert(!IsCreated());
	curr_record = db_table->getCurrentRecordNo();
	records_count = db_table->GetRecordsCount();
	auto nav_label = ConvToStr();

	XEdit::Create(parent, flags | FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED | \
									FL_EDIT_AUTOHSCROLL | FL_EDIT_READONLY | \
									FL_EDIT_CENTER, \
					nav_label, x, y, width, height);
}

CDbNavigator::~CDbNavigator() {

	db_table->DisconnectDbEventsHandler(evt_handler);
}
