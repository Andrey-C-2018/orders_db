#pragma once
#include <basic/XConv.h>
#include <db/IDbResultSet.h>
#include <xwindows/XEdit.h>

class CDbStaticNumField : public XEdit {
protected:
	std::shared_ptr<IDbResultSet> rs;
	const size_t field_index;

	virtual const Tchar *ConvToStr() const = 0;

public:
	CDbStaticNumField(std::shared_ptr<IDbResultSet> rs_, \
					const size_t field_index_);

	CDbStaticNumField(const CDbStaticNumField &obj) = delete;
	CDbStaticNumField(CDbStaticNumField &&obj) = default;
	CDbStaticNumField &operator=(const CDbStaticNumField &obj) = delete;
	CDbStaticNumField &operator=(CDbStaticNumField &&obj) = default;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;
	inline void Refresh();
	inline void Refresh(std::shared_ptr<IDbResultSet> rs);

	virtual ~CDbStaticNumField();
};

class CDbStaticIntField : public CDbStaticNumField {

	mutable Tchar buffer[getDigitsCountOfType<size_t>() + 1];
	const Tchar *ConvToStr() const override;

public:
	CDbStaticIntField(std::shared_ptr<IDbResultSet> rs_, \
						const size_t field_index_);

	CDbStaticIntField(const CDbStaticIntField &obj) = delete;
	CDbStaticIntField(CDbStaticIntField &&obj) = default;
	CDbStaticIntField &operator=(const CDbStaticIntField &obj) = delete;
	CDbStaticIntField &operator=(CDbStaticIntField &&obj) = default;

	virtual ~CDbStaticIntField();
};

class CDbStaticDecimalField : public CDbStaticNumField {
	const Tchar *ConvToStr() const override;
public:
	CDbStaticDecimalField(std::shared_ptr<IDbResultSet> rs_, \
							const size_t field_index_);

	CDbStaticDecimalField(const CDbStaticDecimalField &obj) = delete;
	CDbStaticDecimalField(CDbStaticDecimalField &&obj) = default;
	CDbStaticDecimalField &operator=(const CDbStaticDecimalField &obj) = delete;
	CDbStaticDecimalField &operator=(CDbStaticDecimalField &&obj) = default;

	virtual ~CDbStaticDecimalField();
};

//*****************************************************

void CDbStaticNumField::Refresh() {

	rs->reload();
	SetLabel(ConvToStr());
}

void CDbStaticNumField::Refresh(std::shared_ptr<IDbResultSet> rs) {

	this->rs = rs;
	Refresh();
}
