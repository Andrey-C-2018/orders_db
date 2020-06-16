#include "DbStaticNumField.h"

CDbStaticNumField::CDbStaticNumField(std::shared_ptr<IDbResultSet> rs_, \
								const size_t field_index_) : \
							rs(rs_), field_index(field_index_) { }

void CDbStaticNumField::Create(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) {
	
	assert(!IsCreated());

	XEdit::Create(parent, flags | FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED | \
					FL_EDIT_AUTOHSCROLL | FL_EDIT_READONLY | \
					FL_EDIT_CENTER, \
					ConvToStr(), x, y, width, height);
}

CDbStaticNumField::~CDbStaticNumField() { }

//*****************************************************

CDbStaticIntField::CDbStaticIntField(std::shared_ptr<IDbResultSet> rs_, \
												const size_t field_index_) : \
										CDbStaticNumField(rs_, field_index_) { }

const Tchar *CDbStaticIntField::ConvToStr() const {

	assert(!rs->empty());

	size_t size;
	bool is_null;

	rs->gotoRecord(0);
	int value = rs->getInt(field_index, is_null);
	return is_null ? _T("0") : XConv::ToString(value, buffer, size);
}

CDbStaticIntField::~CDbStaticIntField() { }

//*****************************************************

CDbStaticDecimalField::CDbStaticDecimalField(std::shared_ptr<IDbResultSet> rs_, \
											const size_t field_index_) : \
										CDbStaticNumField(rs_, field_index_) { }

const Tchar *CDbStaticDecimalField::ConvToStr() const {

	assert(!rs->empty());

	rs->gotoRecord(0);
	auto value = rs->getWString(field_index);
	return value ? value : _T("0");
}

CDbStaticDecimalField::~CDbStaticDecimalField() { }