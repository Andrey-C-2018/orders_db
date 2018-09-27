#include <algorithm>
#include "StringTable.h"

CStringTable::CStringTable() {

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

void CStringTable::ConnectOnSizeChangedHandler(ITableOnSizeChangedHandlerPtr handler) {

	on_size_changed_handlers.push_back(handler);
}

void CStringTable::AddField(const size_t max_field_len, const Tchar *field_name) {
	CTextVector new_field;

	size_t records_count = 0;
	if (!text_table.empty()) {
		records_count = text_table[0].size();

		new_field.resize(records_count);
	}
	text_table.push_back(new_field);
	fields_lengthes.push_back(max_field_len);

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
