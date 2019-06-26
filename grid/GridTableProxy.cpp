#include "GridTableProxy.h"
#include "IReloadable.h"

CGridTableProxy::CGridTableProxy(std::shared_ptr<ITable> table_, \
									IReloadable *table_widget_) : \
										table(table_), table_widget(table_widget_), \
										records_count(0) {

	assert(table_widget);
	fields_props = std::make_shared<CFieldsProperties>(table);
	records_count = table->GetRecordsCount();
}

void CGridTableProxy::OnFieldsCountChanged(const size_t new_fields_count) {

	if (new_fields_count == fields_props->GetFieldsCount() + 1) {
		assert(new_fields_count != 0);
		fields_props->OnFieldAddedToDataTable(new_fields_count - 1);
	}
	else
		fields_props->SyncWithDataTable();
	table_widget->Reload();
}

void CGridTableProxy::OnRecordsCountChanged(const size_t new_records_count) {

	records_count = new_records_count;
	table_widget->Reload();
}

void CGridTableProxy::OnTableReset() {

	fields_props->SyncWithDataTable();
	records_count = table->GetRecordsCount();
	table_widget->Reload();
}

void CGridTableProxy::SetFieldWidth(const size_t field, const int new_width) {

	fields_props->SetWidth(field, new_width);
}

void CGridTableProxy::SetFieldName(const size_t field, const Tchar *new_name) {

	fields_props->SetFieldName(field, new_name);
}

void CGridTableProxy::HideField(const size_t field) {

	fields_props->HideField(field);
	table_widget->Reload();
}

CGridTableProxy::~CGridTableProxy() { }