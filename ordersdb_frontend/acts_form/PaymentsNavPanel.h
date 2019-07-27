#pragma once
#include <xwindows_ex/XTabStopPanel.h>
#include <xwindows_ex/XTabStopEdit.h>

struct IDbConnection;
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

	XTabStopEdit *age, *inv, *ill, *zek, *vpr;
	XTabStopEdit *reduce, *change, *close, *zv;
	XTabStopEdit *min_penalty, *nm_suv, *zv_kr, *no_ch_Ist;
	XTabStopEdit *Koef;
	CDbComboBox *checker;

	XButton *btn_get_curr, *btn_add, *btn_remove;
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

	virtual ~CPaymentsNavPanel();
};

