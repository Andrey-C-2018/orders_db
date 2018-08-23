#include <algorithm>
#include "StringTable.h"

CFieldsProperties::CFieldsProperties() { }

void CFieldsProperties::AddField(const int field_size, const Tchar *field_name) {

	assert(field_name);
	fields_props.push_back(FieldProps{ field_size, field_name });
}

int CFieldsProperties::GetWidth(const size_t index) const {

	assert(index < fields_props.size());
	return fields_props[index].field_size;
}

ImmutableString CFieldsProperties::GetName(const size_t index) const {

	assert(index < fields_props.size());

	ImmutableString field_name(fields_props[index].field_name.c_str(), \
								fields_props[index].field_name.size());
	return field_name;
}

size_t CFieldsProperties::size() const {
	
	return fields_props.size();
}

int CFieldsProperties::GetSizesSumm() const {
	int summ = 0;

	std::for_each(fields_props.begin(), fields_props.end(), \
					[&summ](const FieldProps &props) {
					summ += props.field_size;
				});
	return summ;
}

CFieldsProperties::~CFieldsProperties() { }

//**************************************************************

CStringTable::CStringTable() : fields_props(std::make_shared<CFieldsProperties>()){

	on_size_changed_handlers.reserve(DEF_ON_CHANGE_HANDLERS_COUNT);
}

bool CStringTable::empty() const {
	
	return text_table.empty();
}

size_t CStringTable::GetFieldsCount() const {

	return text_table.size();
}

size_t CStringTable::GetRecordsCount() const {

	return text_table.empty() ? 0 : text_table[0].size();
}

std::shared_ptr<const IFieldsProperties> CStringTable::GetFieldsProperties() const {

	return fields_props;
}

void CStringTable::ConnectOnSizeChangedHandler(ITableOnSizeChangedHandlerPtr handler) {

	on_size_changed_handlers.push_back(handler);
}

void CStringTable::AddField(const int field_size, const Tchar *field_name) {
	CTextVector new_field;

	size_t records_count = 0;
	if (!text_table.empty()) {
		records_count = text_table[0].size();

		new_field.resize(records_count);
	}
	text_table.push_back(new_field);
	fields_props->AddField(field_size, field_name);

	for (auto & handler : on_size_changed_handlers)
		handler->OnSizeChanged(text_table.size(), records_count);
}

void CStringTable::AddRecord() {

	std::for_each(text_table.begin(), text_table.end(), \
					[](CTextVector &field) {

					field.push_back(Tstring());
				});

	size_t records_count = text_table.empty() ? 0 : text_table[0].size();

	for (auto & handler : on_size_changed_handlers)
		handler->OnSizeChanged(text_table.size(), records_count);
}

ImmutableString CStringTable::GetCellAsString(const size_t field, \
												const size_t record) const {

	if (text_table.empty() || \
		!(field < text_table.size() && record < text_table[0].size()))
		throw XException(0, _T("The index is out of range"));

	return ImmutableString(text_table[field][record]);
}

void CStringTable::SetCell(const size_t field, const size_t record, \
							const Tchar *value) {

	if (text_table.empty() || \
		!(field < text_table.size() && record < text_table[0].size()))
		throw XException(0, _T("The index is out of range"));

	text_table[field][record] = value;
}

CStringTable::~CStringTable() { }
