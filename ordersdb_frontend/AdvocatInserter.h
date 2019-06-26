#pragma once
#include <basic/tstring.h>
#include <date/Date.h>

class XWidget;
class XComboBox;
class CDbComboBox;
struct IDbConnection;
struct IDbStatement;

class CAdvocatInserter {
	XWidget *id_advocat;
	XWidget *adv_name;
	std::string adv_surname, adv_name_full, adv_patr;
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

	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<IDbStatement> stmt;

	Tstring bindIdAdvocat(size_t &param_no);
	Tstring bindAdvName(size_t &param_no);
	Tstring bindText(size_t &param_no, XWidget *text_holder, const Tchar *param_name);
	Tstring bindDate(size_t &param_no, XWidget *date_holder, const Tchar *param_name);
	Tstring bindDbCombo(size_t &param_no, CDbComboBox *db_combo, const Tchar *param_name);
	Tstring bindPostIndex(size_t &param_no, XWidget *post_index_holder);
	Tstring bindOrg(size_t &param_no, XWidget *org_name, XWidget *org_type);
public:
	CAdvocatInserter();

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
	
	void prepare(std::shared_ptr<IDbConnection> conn);
	void insert();
	virtual ~CAdvocatInserter();
};

