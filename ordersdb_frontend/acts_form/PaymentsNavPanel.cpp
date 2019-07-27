#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db_controls/DbComboBox.h>
#include <xwindows/XEdit.h>
#include <xwindows/XButton.h>
#include <xwindows_ex/HorizontalSizer.h>
#include <xwindows_ex/VerticalSizer.h>
#include "PaymentsNavPanel.h"

CPaymentsNavPanel::CPaymentsNavPanel(std::shared_ptr<IDbConnection> conn) : \
					stage(nullptr), informer(nullptr), cycle(nullptr), article(nullptr), \
					fee(nullptr), outgoings(nullptr), id_act(nullptr), act_date(nullptr), \
					act_reg_date(nullptr), btn_get_curr(nullptr), btn_add(nullptr), \
					btn_remove(nullptr), age(nullptr), inv(nullptr), ill(nullptr), \
					zek(nullptr), vpr(nullptr), reduce(nullptr), change(nullptr), \
					close(nullptr), zv(nullptr), min_penalty(nullptr), nm_suv(nullptr), \
					zv_kr(nullptr), no_ch_Ist(nullptr), Koef(nullptr), checker(nullptr) {

	try {
		auto stmt = conn->PrepareQuery("SELECT id_st,stage_name FROM stages ORDER BY id_st");
		auto rs = stmt->exec();
		stage = new CDbComboBox(rs, 1, 0);

		stmt = conn->PrepareQuery("SELECT id_inf,informer_name FROM informers ORDER BY id_inf");
		rs = stmt->exec();
		informer = new CDbComboBox(rs, 1, 0);

		stmt = conn->PrepareQuery("SELECT id_user,user_name FROM users ORDER BY user_name");
		rs = stmt->exec();
		checker = new CDbComboBox(rs, 1, 0);

		cycle = new XTabStopEdit(this);
		article = new XTabStopEdit(this);
		fee = new XTabStopEdit(this);
		outgoings = new XTabStopEdit(this);

		id_act = new XTabStopEdit(this);
		act_date = new XTabStopEdit(this);
		act_reg_date = new XTabStopEdit(this);

		btn_get_curr = new XButton();
		btn_add = new XButton();
		btn_remove = new XButton();

		age = new XTabStopEdit(this);
		inv = new XTabStopEdit(this);
		ill = new XTabStopEdit(this);
		zek = new XTabStopEdit(this);
		vpr = new XTabStopEdit(this);
		reduce = new XTabStopEdit(this);
		change = new XTabStopEdit(this);
		close = new XTabStopEdit(this);
		zv = new XTabStopEdit(this);
		min_penalty = new XTabStopEdit(this);
		nm_suv = new XTabStopEdit(this);
		zv_kr = new XTabStopEdit(this);
		no_ch_Ist = new XTabStopEdit(this);
		Koef = new XTabStopEdit(this);
	}
	catch (...) {

		Dispose();
		throw;
	}
}

void CPaymentsNavPanel::Create(XWindow *parent, const int flags, \
								const Tchar *label, \
								const int x, const int y, \
								const int width, const int height) {
	const int DEF_HEIGHT = 25, VERT_GAP = 10, HORZ_GAP = 5;
	const int edit_flags = FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED | \
							FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL;

	XTabStopPanel::Create(parent, flags | FL_WINDOW_BORDERED, _T(""), x, y, width, height);

	CVerticalSizer main_sizer(this, 0, 0, 2000, height, 0, 0, 0, 0, HORZ_GAP, DEF_HEIGHT);
	CHorizontalSizer sizer(CSizerPreferences(0, 0, 0, 0, HORZ_GAP));
	
	main_sizer.pushNestedSizer(sizer);
	sizer.addWidget(btn_get_curr, _T("->"), FL_WINDOW_VISIBLE, XSize(20, DEF_HEIGHT));
	sizer.addResizeableWidget(stage, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
								XSize(90, DEF_HEIGHT), 150);
	stage->setTabStopManager(this);

	sizer.addWidget(cycle, _T(""), edit_flags, XSize(25, DEF_HEIGHT));
	sizer.addWidget(article, _T(""), edit_flags, XSize(150, DEF_HEIGHT));
	sizer.addWidget(fee, _T(""), edit_flags, XSize(70, DEF_HEIGHT));
	sizer.addWidget(outgoings, _T(""), edit_flags, XSize(70, DEF_HEIGHT));
	sizer.addResizeableWidget(informer, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
								XSize(180, DEF_HEIGHT), 150);
	informer->setTabStopManager(this);

	sizer.addWidget(id_act, _T(""), edit_flags, XSize(80, DEF_HEIGHT));
	sizer.addWidget(act_date, _T(""), edit_flags, XSize(80, DEF_HEIGHT));
	sizer.addWidget(act_reg_date, _T(""), edit_flags, XSize(80, DEF_HEIGHT));

	sizer.addWidget(btn_add, _T("+"), FL_WINDOW_VISIBLE, XSize(20, DEF_HEIGHT));
	main_sizer.popNestedSizer();

	//main_sizer.pushNestedSizer(sizer);

	//main_sizer.popNestedSizer();
}

void CPaymentsNavPanel::Dispose() {


	if (Koef && !Koef->IsCreated()) delete Koef;
	Koef = nullptr;
	if (no_ch_Ist && !no_ch_Ist->IsCreated()) delete no_ch_Ist;
	no_ch_Ist = nullptr;
	if (zv_kr && !zv_kr->IsCreated()) delete zv_kr;
	zv_kr = nullptr;
	if (nm_suv && !nm_suv->IsCreated()) delete nm_suv;
	nm_suv = nullptr;
	if (min_penalty && !min_penalty->IsCreated()) delete min_penalty;
	min_penalty = nullptr;
	if (zv && !zv->IsCreated()) delete zv;
	zv = nullptr;
	if (close && !close->IsCreated()) delete close;
	close = nullptr;
	if (change && !change->IsCreated()) delete change;
	change = nullptr;
	if (reduce && !reduce->IsCreated()) delete reduce;
	reduce = nullptr;
	if (vpr && !vpr->IsCreated()) delete vpr;
	vpr = nullptr;
	if (zek && !zek->IsCreated()) delete zek;
	zek = nullptr;
	if (ill && !ill->IsCreated()) delete ill;
	ill = nullptr;
	if (inv && !inv->IsCreated()) delete inv;
	inv = nullptr;
	if (age && !age->IsCreated()) delete age;
	age = nullptr;

	if (btn_add && !btn_add->IsCreated()) delete btn_add;
	btn_add = nullptr;
	if (btn_remove && !btn_remove->IsCreated()) delete btn_remove;
	btn_remove = nullptr;
	if (btn_get_curr && !btn_get_curr->IsCreated()) delete btn_get_curr;
	btn_get_curr = nullptr;

	if (act_reg_date && !act_reg_date->IsCreated()) delete act_reg_date;
	act_reg_date = nullptr;
	if (act_date && !act_date->IsCreated()) delete act_date;
	act_date = nullptr;
	if (id_act && !id_act->IsCreated()) delete id_act;
	id_act = nullptr;
	if (outgoings && !outgoings->IsCreated()) delete outgoings;
	outgoings = nullptr;
	if (fee && !fee->IsCreated()) delete fee;
	fee = nullptr;
	if (article && !article->IsCreated()) delete article;
	article = nullptr;
	if (cycle && !cycle->IsCreated()) delete cycle;
	cycle = nullptr;

	if (checker && !checker->IsCreated()) delete checker;
	checker = nullptr;
	if (informer && !informer->IsCreated()) delete informer;
	informer = nullptr;
	if (stage && !stage->IsCreated()) delete stage;
	stage = nullptr;
}

CPaymentsNavPanel::~CPaymentsNavPanel() {

	Dispose();
}
