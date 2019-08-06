#pragma once
#include <db_ext/DbInserter.h>
#include <db_controls/InsertionBinders.h>

class XComboBox;

class CAdvocatInserter : public CDbInserter {
	enum {
		FIELDS_COUNT = 15
	};
	std::shared_ptr<IDbConnection> conn;

	XWidget *id_advocat;
	XWidget *adv_name;
	XWidget *license_no;
	XWidget *license_date;
	CDbComboBox *examiner;
	XWidget *post_index;
	XWidget *address;
	XWidget *tel;
	XWidget *email;
	XWidget *adv_bdate;
	CDbComboBox *district;
	XWidget *org_name;
	XComboBox *org_type;

public:
	CAdvocatInserter();

	CAdvocatInserter(const CAdvocatInserter &obj) = delete;
	CAdvocatInserter(CAdvocatInserter &&obj) = delete;
	CAdvocatInserter &operator=(const CAdvocatInserter &obj) = delete;
	CAdvocatInserter &operator=(CAdvocatInserter &&obj) = delete;

	inline void setDbConnection(std::shared_ptr<IDbConnection> conn);

	inline void setIdAdvocatWidget(XWidget *id_advocat) { this->id_advocat = id_advocat; }
	inline void setAdvNameWidget(XWidget *adv_name) { this->adv_name = adv_name; }
	inline void setLicenseNoWidget(XWidget *license_no) { this->license_no = license_no; }
	inline void setLicenseDateWidget(XWidget *license_date) { this->license_date = license_date; }
	inline void setExaminerWidget(CDbComboBox *examiner) { this->examiner = examiner; }
	inline void setPostIndexWidget(XWidget *post_index) { this->post_index = post_index; }
	inline void setAddressWidget(XWidget *address) { this->address = address; }
	inline void setBDateWidget(XWidget *bdate) { this->adv_bdate = bdate; }
	inline void setTelWidget(XWidget *tel) { this->tel = tel; }
	inline void setEmailWidget(XWidget *email) { this->email = email; }
	inline void setDistrictWidget(CDbComboBox *district) { this->district = district; }
	inline void setOrgNameWidget(XWidget *org_name) { this->org_name = org_name; }
	inline void setOrgTypeWidget(XComboBox *org_type) { this->org_type = org_type; }

	void prepare(std::shared_ptr<IDbConnection> conn) override;
	void insert() override;

	virtual ~CAdvocatInserter();
};

//*****************************************************

void CAdvocatInserter::setDbConnection(std::shared_ptr<IDbConnection> conn) {

	assert(!this->conn);
	this->conn = conn;
}