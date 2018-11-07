#include <algorithm>
#include "StringTable.h"

CStringTable::CStringTable() {

	event_handlers.reserve(DEF_EVENT_HANDLERS_COUNT);
	fields.reserve(DEF_FIELDS_COUNT);
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

void CStringTable::ConnectEventsHandler(ITableEventsHandlerPtr handler) {

	event_handlers.push_back(handler);
}

void CStringTable::DisconnectEventsHandler(ITableEventsHandlerPtr handler) {

	auto p = std::find(event_handlers.begin(), event_handlers.end(), handler);
	assert(p != event_handlers.end());

	event_handlers.erase(p);
}

void CStringTable::AddField(const size_t max_field_len, const Tchar *field_name) {
	CTextVector new_field;

	size_t records_count = 0;
	if (!text_table.empty()) {
		records_count = text_table[0].size();

		new_field.resize(records_count);
	}
	text_table.push_back(new_field);

	CField rec;
	rec.field_name = field_name;
	rec.max_size_in_chars = max_field_len;
	fields.emplace_back(rec);

	for (auto & handler : event_handlers)
		handler->OnFieldsCountChanged(text_table.size());
}

void CStringTable::AddRecord() {

	std::for_each(text_table.begin(), text_table.end(), \
					[](CTextVector &field) {

					field.push_back(Tstring());
				});

	size_t records_count = text_table.empty() ? 0 : text_table[0].size();

	for (auto & handler : event_handlers)
		handler->OnRecordsCountChanged(records_count);
}

size_t CStringTable::GetFieldMaxLengthInChars(const size_t field) const {

	assert(field < fields.size());
	return fields[field].max_size_in_chars;
}

const Tchar *CStringTable::GetFieldName(const size_t field) const {

	assert(field < fields.size());
	return fields[field].field_name.c_str();
}

ImmutableString<Tchar> CStringTable::GetFieldNameAsImmutableStr(const size_t field) const {

	assert(field < fields.size());
	return ImmutableString<Tchar>(fields[field].field_name.c_str(), \
									fields[field].field_name.size());
}

ImmutableString<Tchar> CStringTable::GetCellAsString(const size_t field, \
												const size_t record) const {

	if (text_table.empty() || \
		!(field < text_table.size() && record < text_table[0].size()))
		throw XException(0, _T("The index is out of range"));

	return ImmutableString<Tchar>(text_table[field][record]);
}

void CStringTable::SetCell(const size_t field, const size_t record, \
							const Tchar *value) {

	if (text_table.empty() || \
		!(field < text_table.size() && record < text_table[0].size()))
		throw XException(0, _T("The index is out of range"));

	text_table[field][record] = value;
}

CStringTable::~CStringTable() { }
