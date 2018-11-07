#include "TableEventsHandler.h"
#include "FieldsProperties.h"
#include "IReloadable.h"

CDataTableEventsHandler::CDataTableEventsHandler(IReloadable *grid_, \
							std::shared_ptr<CFieldsProperties> fields_props_) : \
							grid(grid_), fields_props(fields_props_), \
							records_count(0) { }

void CDataTableEventsHandler::OnFieldsCountChanged(const size_t new_fields_count) {

	fields_props->OnFieldsCountChanged(new_fields_count);
	grid->Reload();
}

void CDataTableEventsHandler::OnRecordsCountChanged(const size_t new_records_count) {

	records_count = new_records_count;
	grid->Reload();
}

void CDataTableEventsHandler::OnTableReset() {

	fields_props->OnTableReset();
	grid->Reload();
}

CDataTableEventsHandler::~CDataTableEventsHandler() { }