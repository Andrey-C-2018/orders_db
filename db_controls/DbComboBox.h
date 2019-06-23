#pragma once
#include <xwindows/XComboBox.h>

struct IDbResultSet;

class CDbComboBox : public XComboBox {
	std::shared_ptr<const IDbResultSet> result_set;
	size_t field_to_display;
	
	void fillComboBox();
	void OnItemChoosed(XCommandEvent *eve);

public:
	CDbComboBox(std::shared_ptr<const IDbResultSet> result_set, \
				const size_t field_to_display);

	CDbComboBox(const CDbComboBox &obj) = delete;
	CDbComboBox(CDbComboBox &&obj) = default;
	CDbComboBox &operator=(const CDbComboBox &obj) = delete;
	CDbComboBox &operator=(CDbComboBox &&obj) = default;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	inline std::shared_ptr<const IDbResultSet> getResultSet() const;

	virtual ~CDbComboBox();
};

//**************************************************************

std::shared_ptr<const IDbResultSet> CDbComboBox::getResultSet() const {

	return result_set;
}