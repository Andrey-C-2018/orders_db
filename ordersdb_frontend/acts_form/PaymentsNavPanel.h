#pragma once
#include <xwindows_ex/XTabStopPanel.h>
#include <xwindows_ex/XTabStopEdit.h>

struct IDbConnection;
struct IDbStatement;
class CDbTable;
class XButton;
class CDbComboBox;

class CPaymentsNavPanel : public XTabStopPanel {
	CDbComboBox *stage;
	XTabStopEdit *cycle;
	XTabStopEdit *article;
	XTabStopEdit *fee;
	XTabStopEdit *outgoings;
	CDbComboBox *informer;
	XTabStopEdit *id_act;
	XTabStopEdit *act_date;
	XTabStopEdit *act_reg_date;

	XTabStopEdit *age, *inv, *lang, *ill, *zek, *vpr;
	XTabStopEdit *reduce, *change, *close, *zv;
	XTabStopEdit *min_penalty, *nm_suv, *zv_kr, *no_ch_Ist;
	XTabStopEdit *Koef;
	CDbComboBox *checker;

	XButton *btn_get_curr, *btn_add, *btn_remove;

	std::shared_ptr<IDbStatement> stmt_stages, stmt_inf, stmt_checkers;
	std::shared_ptr<CDbTable> db_table;

	void OnGetCurrRecordButtonClick(XCommandEvent *eve);
	void OnAddRecordButtonClick(XCommandEvent *eve);
	void OnRemoveButtonClick(XCommandEvent *eve);
	void Dispose();

public:
	CPaymentsNavPanel(std::shared_ptr<IDbConnection> conn);

	CPaymentsNavPanel(CPaymentsNavPanel &&obj) = default;
	CPaymentsNavPanel(const CPaymentsNavPanel &obj) = delete;
	CPaymentsNavPanel &operator=(const CPaymentsNavPanel &obj) = delete;
	CPaymentsNavPanel &operator=(CPaymentsNavPanel &&obj) = default;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	inline void setPaymentsDbTable(std::shared_ptr<CDbTable> db_table);
	inline std::shared_ptr<IDbStatement> getStagesListStmt();
	inline std::shared_ptr<IDbStatement> getInformersListStmt();
	inline std::shared_ptr<IDbStatement> getCheckersListStmt();

	virtual ~CPaymentsNavPanel();
};

//*****************************************************

void CPaymentsNavPanel::setPaymentsDbTable(std::shared_ptr<CDbTable> db_table) {

	this->db_table = db_table;
}

std::shared_ptr<IDbStatement> CPaymentsNavPanel::getStagesListStmt() {

	return stmt_stages;
}

std::shared_ptr<IDbStatement> CPaymentsNavPanel::getInformersListStmt() {

	return stmt_inf;
}

std::shared_ptr<IDbStatement> CPaymentsNavPanel::getCheckersListStmt() {

	return stmt_checkers;
}