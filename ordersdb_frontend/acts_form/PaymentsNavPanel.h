#pragma once
#include <xwindows_ex/XTabStopPanel.h>
#include <xwindows_ex/XTabStopEdit.h>
#include "PaymentsInserter.h"

struct IDbResultSet;
class CDbTable;
class XButton;

class CPaymentsNavPanel : public XTabStopPanel {
	
	XButton *btn_get_curr, *btn_add, *btn_remove;
	std::shared_ptr<CDbTable> db_table;

	std::shared_ptr<const IDbResultSet> rs_stages;
	std::shared_ptr<const IDbResultSet> rs_inf;
	std::shared_ptr<const IDbResultSet> rs_checkers;

	CPaymentsInserter inserter;

	void OnGetCurrRecordButtonClick(XCommandEvent *eve);
	void OnAddRecordButtonClick(XCommandEvent *eve);
	void OnRemoveButtonClick(XCommandEvent *eve);

public:
	CPaymentsNavPanel(std::shared_ptr<IDbConnection> conn, \
						std::shared_ptr<const IDbResultSet> rs_stages, \
						std::shared_ptr<const IDbResultSet> rs_inf, \
						std::shared_ptr<const IDbResultSet> rs_checkers);

	CPaymentsNavPanel(CPaymentsNavPanel &&obj) = default;
	CPaymentsNavPanel(const CPaymentsNavPanel &obj) = delete;
	CPaymentsNavPanel &operator=(const CPaymentsNavPanel &obj) = delete;
	CPaymentsNavPanel &operator=(CPaymentsNavPanel &&obj) = default;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	inline void setPaymentsDbTable(std::shared_ptr<CDbTable> db_table);

	virtual ~CPaymentsNavPanel();
};

//*****************************************************

void CPaymentsNavPanel::setPaymentsDbTable(std::shared_ptr<CDbTable> db_table) {

	this->db_table = db_table;
}