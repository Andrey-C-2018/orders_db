#pragma once
#include <db_ext/IDbInserter.h>
#include <db_ext/DbInsertHelper.h>

struct IDbStatement;
class XComboBox;
class CDbComboBox;

class CAdvocatInserter : public IDbInserter {
	enum {
        PEOPLE_FIELDS_COUNT = 3,
		ADV_FIELDS_COUNT = 11
	};
	CDbInsertHelper people_ins_helper, adv_ins_helper;
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<IDbStatement> stmt_adv, stmt_people;

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
	bool insert() override;

	virtual ~CAdvocatInserter();
};
