#include <db/IDbConnection.h>
#include <db_ext/DbTable.h>
#include <db_controls/DbComboBox.h>
#include <xwindows/XButton.h>
#include <xwindows/XLabel.h>
#include <xwindows_ex/HorizontalSizer.h>
#include <xwindows_ex/VerticalSizer.h>
#include <xwindows_ex/XNullWidget.h>
#include <xwindows_ex/XDateField.h>
#include <xwindows_ex/XCurrencyField.h>
#include "PaymentsNavPanel.h"
#include "Constraints.h"

CPaymentsNavPanel::CPaymentsNavPanel(std::shared_ptr<IDbConnection> conn_, \
										std::shared_ptr<CDbTable> db_table_, \
										std::shared_ptr<CPaymentsConstraints> constraints_, \
										std::shared_ptr<const IDbResultSet> rs_stages_, \
										std::shared_ptr<const IDbResultSet> rs_inf_, \
										std::shared_ptr<const IDbResultSet> rs_checkers_) : \
					btn_get_curr(nullptr), btn_add(nullptr), btn_remove(nullptr), \
					conn(conn_), db_table(db_table_), constraints(constraints_), \
					rs_stages(rs_stages_), rs_inf(rs_inf_), rs_checkers(rs_checkers_), \
					inserter(db_table_) {

	btn_get_curr = new XButton();
	btn_add = new XButton();
	btn_remove = new XButton();
}

void CPaymentsNavPanel::Create(XWindow *parent, const int flags, \
								const Tchar *label, \
								const int x, const int y, \
								const int width, const int height) {
	const int DEF_HEIGHT = 25, VERT_GAP = 10, HORZ_GAP = 5, QA_ITEM_WIDTH = 15;
	const int QA_LABELS_HEIGHT = 15;
	const int edit_flags = FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED | \
							FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL | \
							FL_WINDOW_CLIPCHILDREN;
	const int label_flags = FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN;

	XTabStopPanel::Create(parent, flags | FL_WINDOW_BORDERED | FL_WINDOW_CLIPCHILDREN, \
							_T(""), x, y, width, height);

	CVerticalSizer main_sizer(this, 0, 0, 2000, height, 0, 0, 0, 0, HORZ_GAP, DEF_HEIGHT);
	CHorizontalSizer sizer(CSizerPreferences(0, 0, 0, 0, HORZ_GAP));
	
	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(btn_get_curr, _T("->"), FL_WINDOW_VISIBLE, XSize(20, DEF_HEIGHT));

		auto stage = new CDbComboBox(rs_stages, 1, 0);
		inserter.setStageWidget(stage);
		sizer.addResizeableWidget(stage, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
									XSize(90, DEF_HEIGHT), 150);
		stage->setTabStopManager(this);

		auto cycle = new XTabStopEdit(this);
		inserter.setCycleWidget(cycle);
		sizer.addWidget(cycle, _T(""), edit_flags, XSize(25, DEF_HEIGHT));

		auto article = new XTabStopEdit(this);
		inserter.setArticleWidget(article);
		sizer.addWidget(article, _T(""), edit_flags, XSize(150, DEF_HEIGHT));

		auto fee = new XCurrencyField(this);
		inserter.setFeeWidget(fee);
		sizer.addWidget(fee, _T(""), edit_flags, XSize(70, DEF_HEIGHT));

		auto outgoings = new XCurrencyField(this);
		inserter.setOutgoingsWidget(outgoings);
		sizer.addWidget(outgoings, _T(""), edit_flags, XSize(70, DEF_HEIGHT));

		auto informer = new CDbComboBox(rs_inf, 1, 0);
		inserter.setInformerWidget(informer);
		sizer.addResizeableWidget(informer, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
									XSize(180, DEF_HEIGHT), 150);
		informer->setTabStopManager(this);

		auto id_act = new XTabStopEdit(this);
		inserter.setActWidget(id_act);
		sizer.addWidget(id_act, _T(""), edit_flags, XSize(80, DEF_HEIGHT));

		auto act_date = new XDateField(this);
		inserter.setActDateWidget(act_date);
		sizer.addWidget(act_date, _T(""), edit_flags, XSize(80, DEF_HEIGHT));

		auto act_reg_date = new XDateField(this);
		inserter.setActRegDateWidget(act_reg_date);
		sizer.addWidget(act_reg_date, _T(""), edit_flags, XSize(80, DEF_HEIGHT));

		sizer.addWidget(btn_add, _T("+"), FL_WINDOW_VISIBLE, XSize(20, DEF_HEIGHT));
	main_sizer.popNestedSizer();

	XNullWidget null_widget;
	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(&null_widget, _T(""), 0, XSize(20, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Вік"), label_flags, XSize(25, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Вади"), label_flags, XSize(35, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Мова"), label_flags, XSize(35, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Хвороба"), label_flags, XSize(58, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("ЗЕК"), label_flags, XSize(30, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Випр."), label_flags, XSize(40, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Зменш"), label_flags, XSize(47, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Зміна"), label_flags, XSize(40, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Закр."), label_flags, XSize(40, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Звільн"), label_flags, XSize(55, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Мін."), label_flags, XSize(35, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Найм. сув."), label_flags, XSize(70, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Звільн. кр"), label_flags, XSize(70, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Без зм 1 інст"), label_flags, XSize(90, QA_LABELS_HEIGHT));
		sizer.addWidget(new XLabel(), _T("Кзвіт"), label_flags, XSize(40, QA_LABELS_HEIGHT));
		sizer.addWidget(&null_widget, _T(""), 0, XSize(80, QA_LABELS_HEIGHT));
		sizer.addWidget(btn_remove, _T("-"), FL_WINDOW_VISIBLE, XSize(20, QA_LABELS_HEIGHT));
	main_sizer.popNestedSizer();

	main_sizer.pushNestedSizer(sizer);
		sizer.addWidget(&null_widget, _T(""), 0, XSize(20, DEF_HEIGHT));

		auto age = new XTabStopEdit(this);
		inserter.setAgeWidget(age);
		sizer.addWidget(age, _T(""), edit_flags, XSize(25, DEF_HEIGHT));

		auto inv = new XTabStopEdit(this);
		inserter.setInvWidget(inv);
		sizer.addWidget(inv, _T(""), edit_flags, XSize(35, DEF_HEIGHT));

		auto lang = new XTabStopEdit(this);
		inserter.setLangWidget(lang);
		sizer.addWidget(lang, _T(""), edit_flags, XSize(35, DEF_HEIGHT));

		auto ill = new XTabStopEdit(this);
		inserter.setIllWidget(ill);
		sizer.addWidget(ill, _T(""), edit_flags, XSize(58, DEF_HEIGHT));

		auto zek = new XTabStopEdit(this);
		inserter.setZekWidget(zek);
		sizer.addWidget(zek, _T(""), edit_flags, XSize(30, DEF_HEIGHT));

		auto vpr = new XTabStopEdit(this);
		inserter.setVprWidget(vpr);
		sizer.addWidget(vpr, _T(""), edit_flags, XSize(40, DEF_HEIGHT));

		auto reduce = new XTabStopEdit(this);
		inserter.setReduceWidget(reduce);
		sizer.addWidget(reduce, _T(""), edit_flags, XSize(47, DEF_HEIGHT));

		auto change = new XTabStopEdit(this);
		inserter.setChangeWidget(change);
		sizer.addWidget(change, _T(""), edit_flags, XSize(40, DEF_HEIGHT));

		auto close = new XTabStopEdit(this);
		inserter.setCloseWidget(close);
		sizer.addWidget(close, _T(""), edit_flags, XSize(40, DEF_HEIGHT));

		auto zv = new XTabStopEdit(this);
		inserter.setZvilnWidget(zv);
		sizer.addWidget(zv, _T(""), edit_flags, XSize(55, DEF_HEIGHT));

		auto min_penalty = new XTabStopEdit(this);
		inserter.setMinPenaltyWidget(min_penalty);
		sizer.addWidget(min_penalty, _T(""), edit_flags, XSize(35, DEF_HEIGHT));

		auto nm_suv = new XTabStopEdit(this);
		inserter.setNmSuvWidget(nm_suv);
		sizer.addWidget(nm_suv, _T(""), edit_flags, XSize(70, DEF_HEIGHT));

		auto zv_kr = new XTabStopEdit(this);
		inserter.setZvilnKrWidget(zv_kr);
		sizer.addWidget(zv_kr, _T(""), edit_flags, XSize(70, DEF_HEIGHT));

		auto no_ch_Ist = new XTabStopEdit(this);
		inserter.setNoCh1instWidget(no_ch_Ist);
		sizer.addWidget(no_ch_Ist, _T(""), edit_flags, XSize(90, DEF_HEIGHT));

		auto Koef = new XTabStopEdit(this);
		inserter.setKoeffWidget(Koef);
		sizer.addWidget(Koef, _T(""), edit_flags, XSize(40, DEF_HEIGHT));
		
		auto checker = new CDbComboBox(rs_checkers, 1, 0);
		inserter.setCheckerWidget(checker);
		sizer.addResizeableWidget(checker, _T(""), FL_WINDOW_VISIBLE | FL_WINDOW_BORDERED, \
									XSize(108, DEF_HEIGHT), 200);
		checker->setTabStopManager(this);
	main_sizer.popNestedSizer();

	inserter.prepare(conn);

	Connect(EVT_COMMAND, btn_get_curr->GetId(), this, \
			&CPaymentsNavPanel::OnGetCurrRecordButtonClick);
	Connect(EVT_COMMAND, btn_add->GetId(), this, \
			&CPaymentsNavPanel::OnAddRecordButtonClick);
	Connect(EVT_COMMAND, btn_remove->GetId(), this, \
			&CPaymentsNavPanel::OnRemoveButtonClick);
}

void CPaymentsNavPanel::OnGetCurrRecordButtonClick(XCommandEvent *eve) {

	inserter.getCurrRecord();
}

void CPaymentsNavPanel::OnAddRecordButtonClick(XCommandEvent *eve) {

	inserter.insert();
	db_table->reload();
}

void CPaymentsNavPanel::OnRemoveButtonClick(XCommandEvent *eve) {

	if (db_table->empty()) {
		ErrorBox(_T("Видалення неможливе: відсутні стадії"));
		return;
	}

	if (db_table->GetRecordsCount() == 1) {
		ErrorBox(_T("Доручення містить лише одну стадію, її видалення неможливе"));
		return;
	}

	if (constraints->old_stage_locked) {
		ErrorBox(_T("Неможливо видалити цю стадію, оскільки вона належить до минулого періоду"));
		return;
	}

	if (constraints->wrong_zone) {
		ErrorBox(_T("Неможливо змінити цю стадію, оскільки вона належить до іншого центру"));
		return;
	}
	
	int option = _plMessageBoxYesNo(_T("Видалити поточний запис?"));
	if (option == IDYES)
		db_table->removeCurrentRecord();
}

CPaymentsNavPanel::~CPaymentsNavPanel() {

	if (btn_add && !btn_add->IsCreated()) delete btn_add;
	btn_add = nullptr;

	if (btn_remove && !btn_remove->IsCreated()) delete btn_remove;
	btn_remove = nullptr;

	if (btn_get_curr && !btn_get_curr->IsCreated()) delete btn_get_curr;
	btn_get_curr = nullptr;
}
