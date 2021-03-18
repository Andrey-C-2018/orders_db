#include <db/DbException.h>
#include <db_ext/DbTable.h>
#include <db_ext/InsParamNoGuard.h>
#include <db_controls/DbComboBox.h>
#include <forms_common/FormsInsertionBinders.h>
#include <forms_common/ParametersManager.h>
#include "PaymentsInserter.h"

class COrderParamsBinder : public IInsertBinder {
	std::shared_ptr<CDbTable> db_table;
	size_t order_params[3];
public:
	COrderParamsBinder(std::shared_ptr<CDbTable> db_table_) : db_table(db_table_) {

		const CMetaInfo &meta_info = db_table->getQuery().getMetaInfo();
		order_params[0] = meta_info.getFieldIndexByName("id_center");
		order_params[1] = meta_info.getFieldIndexByName("id_order");
		order_params[2] = meta_info.getFieldIndexByName("order_date");
	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 3);

		auto rs = db_table->getResultSet();
		if (rs->empty()) {
			params.error_str += _T("У дорученні відсутні стадії\n");
			return true;
		}
		db_table->gotoCurrentRecord();

		bool is_null;
		int v = rs->getInt(order_params[0], is_null);
		assert(!is_null);
		binding_target->bindValue(params.param_no, v);

		v = rs->getInt(order_params[1], is_null);
		assert(!is_null);
		binding_target->bindValue(params.param_no + 1, v);

		CDate dt = rs->getDate(order_params[2], is_null);
		assert(!is_null);
		binding_target->bindValue(params.param_no + 2, dt);

		return true;
	}

	virtual ~COrderParamsBinder() { }
};

class CActDateBinder : public CVisualInsertBinder {

	CActDateValidator validator;
public:
	CActDateBinder(std::shared_ptr<CDbTable> db_table_, \
		XWidget *act_date_holder, bool free_widget, \
		XWidget *act_reg_date_holder) : \
		CVisualInsertBinder(act_date_holder, free_widget), \
		validator(db_table_, act_reg_date_holder) {	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
		Params &params, const Tchar *field_name) override {

		CInsParamNoGuard param_no_guard(params.param_no, 1);

		size_t size;
		auto date_str = widget->GetLabel(size);
		if (size == 0) {
			binding_target->bindNull(params.param_no);
			return true;
		}

		size_t err_str_size = params.error_str.size();
		CDate date(date_str, CDate::GERMAN_FORMAT);

		if (!validator.validate(ImmutableString<Tchar>(date_str, size), \
			date, params.error_str, field_name)) return false;

		if (err_str_size == params.error_str.size())
			binding_target->bindValue(params.param_no, date);

		return true;
	}

	virtual ~CActDateBinder() { }
};

//*****************************************************

CPaymentsInserter::CPaymentsInserter(std::shared_ptr<CDbTable> db_table_) : \
				ins_helper(FIELDS_COUNT), db_table(db_table_), \
				stage(nullptr), informer(nullptr), cycle(nullptr), article(nullptr), \
				fee(nullptr), outgoings(nullptr), outg_post(nullptr), outg_daynight(nullptr), \
				id_act(nullptr), act_date(nullptr), act_no(nullptr), \
				act_reg_date(nullptr), age(nullptr), inv(nullptr), lang(nullptr), ill(nullptr), \
				zek(nullptr), vpr(nullptr), reduce(nullptr), change(nullptr), \
				close(nullptr), zv(nullptr), min_penalty(nullptr), nm_suv(nullptr), \
				zv_kr(nullptr), no_ch_Ist(nullptr), Koef(nullptr), checker(nullptr) { }

void CPaymentsInserter::getCurrRecord() {
	enum {
		INT_TYPE_HINT = -1, \
		NULL_CHECKER_INDEX = 0
	};

	assert(db_table);

	auto rs = db_table->getResultSet();
	if (!rs->getRecordsCount()) {
		ErrorBox(_T("Таблиця порожня. Неможливо зчитати поточний запис"));
		return;
	}

	db_table->gotoCurrentRecord();
	auto &meta_info = db_table->getQuery().getMetaInfo();
	stage->SetCurrRecord(rs, meta_info.getFieldIndexByName("id_stage"), INT_TYPE_HINT);

	const size_t BUFFER_SIZE = MAX_INT_FIELD_LEN > CDate::GERMAN_FORMAT_LEN + 1 ? \
													MAX_INT_FIELD_LEN : \
													CDate::GERMAN_FORMAT_LEN + 1;
	Tchar buffer[BUFFER_SIZE];
	setIntWidgetLabel(cycle, rs, meta_info, "cycle", buffer);
	setIntWidgetLabel(act_no, rs, meta_info, "act_no", buffer);

	setStrWidgetLabel(article, rs, meta_info, "article");
	setStrWidgetLabel(fee, rs, meta_info, "fee");
	setStrWidgetLabel(outgoings, rs, meta_info, "outgoings");
	setStrWidgetLabel(outg_post, rs, meta_info, "outg_post");
	setStrWidgetLabel(outg_daynight, rs, meta_info, "outg_daynight");

	informer->SetCurrRecord(rs, meta_info.getFieldIndexByName("id_informer"), INT_TYPE_HINT);

	setStrWidgetLabel(id_act, rs, meta_info, "id_act");
	setDateWidgetLabel(act_date, rs, meta_info, "act_date", buffer);
	setDateWidgetLabel(act_reg_date, rs, meta_info, "act_reg_date", buffer);

	setIntWidgetLabel(age, rs, meta_info, "age", buffer);
	setIntWidgetLabel(inv, rs, meta_info, "inv", buffer);
	setIntWidgetLabel(lang, rs, meta_info, "lang", buffer);
	setIntWidgetLabel(ill, rs, meta_info, "ill", buffer);
	setIntWidgetLabel(zek, rs, meta_info, "zek", buffer);
	setIntWidgetLabel(vpr, rs, meta_info, "vpr", buffer);
	setIntWidgetLabel(reduce, rs, meta_info, "reduce", buffer);
	setIntWidgetLabel(change, rs, meta_info, "change_", buffer);
	setIntWidgetLabel(close, rs, meta_info, "close", buffer);
	setIntWidgetLabel(zv, rs, meta_info, "zv", buffer);
	setIntWidgetLabel(min_penalty, rs, meta_info, "min", buffer);
	setIntWidgetLabel(nm_suv, rs, meta_info, "nm_suv", buffer);
	setIntWidgetLabel(zv_kr, rs, meta_info, "zv_kr", buffer);
	setIntWidgetLabel(no_ch_Ist, rs, meta_info, "No_Ch_Ist", buffer);

	setStrWidgetLabel(Koef, rs, meta_info, "Koef");
	const size_t checker_name_index = meta_info.getFieldIndexByName("user_full_name");
	const char *checker_name = rs->getString(checker_name_index);
	if (checker_name)
		checker->SetCurrRecord(rs, meta_info.getFieldIndexByName("id_checker"), INT_TYPE_HINT);
	else
		checker->SetCurrRecord(NULL_CHECKER_INDEX);
}

void CPaymentsInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	assert(stage);
	assert(cycle);
	assert(act_no);
	assert(article);
	assert(fee);
	assert(outgoings);
	assert(outg_post);
	assert(outg_daynight);

	assert(informer);
	assert(id_act);
	assert(act_date);
	assert(act_reg_date);

	assert(age);
	assert(inv);
	assert(lang);
	assert(ill);
	assert(zek);
	assert(vpr);
	assert(reduce);
	assert(change);
	assert(close);
	assert(zv);
	assert(min_penalty);
	assert(nm_suv);
	assert(zv_kr);
	assert(no_ch_Ist);
	assert(Koef);
	assert(checker);

	auto &params_manager = CParametersManager::getInstance();
	int id_user = params_manager.getIdUser();
	assert(id_user != -1);

	ins_helper.addBinder(0, _T("Реквізити доручення"), \
							std::make_shared<COrderParamsBinder>(db_table), 3);
	ins_helper.addBinder(3, _T("Сума"), \
							std::make_shared<CFeeBinder>(fee, false, false));
	ins_helper.addBinder(4, _T("Стадія"), \
					std::make_shared<CDbComboBoxInsertBinder>(stage, false, false));
	ins_helper.addBinder(5, _T("Стаття"), \
						std::make_shared<UITextInsertBinder>(article, false));
	ins_helper.addBinder(6, _T("Інформатор"), \
				std::make_shared<CDbComboBoxInsertBinder>(informer, false, false));
	ins_helper.addBinder(7, _T("Акт"), \
						std::make_shared<CActNameBinder>(id_act, false));
	ins_helper.addBinder(8, _T("№акту"), \
						std::make_shared<UIIntInsertBinder>(act_no, false));
	ins_helper.addBinder(9, _T("Дата акта"), \
		std::make_shared<CActDateBinder>(db_table, act_date, false, act_reg_date));
	ins_helper.addBinder(10, _T("Цикл"), \
						std::make_shared<UIIntInsertBinder>(cycle, false));
	ins_helper.addBinder(11, _T("Користувач"), \
						std::make_shared<CIntInsertBinder>(id_user));
	ins_helper.defStaticInsertion(12, "NOW()");
	ins_helper.defStaticInsertion(13, "NULL");
	ins_helper.defStaticInsertion(14, "NULL");
	ins_helper.addBinder(15, _T("Дата прийняття акта"), \
			std::make_shared<CActRegDateBinder>(db_table, act_reg_date, false));
	ins_helper.defStaticInsertion(16, "NULL");
	ins_helper.addBinder(17, _T("Витрати"), \
						std::make_shared<CFeeBinder>(outgoings, false, false));
	ins_helper.defStaticInsertion(18, "0.0"); // fee_parus

	ins_helper.addBinder(19, _T("Вік"), std::make_shared<CQaParamBinder>(age, false));
	ins_helper.addBinder(20, _T("Вади"), std::make_shared<CQaParamBinder>(inv, false));
	ins_helper.addBinder(21, _T("Мова"), std::make_shared<CQaParamBinder>(lang, false));
	ins_helper.addBinder(22, _T("Хвороба"), std::make_shared<CQaParamBinder>(ill, false));
	ins_helper.addBinder(23, _T("ЗЕК"), std::make_shared<CQaParamBinder>(zek, false));
	ins_helper.addBinder(24, _T("Випр."), std::make_shared<CQaParamBinder>(vpr, false));
	ins_helper.addBinder(25, _T("Зменш."), std::make_shared<CQaParamBinder>(reduce, false));
	ins_helper.addBinder(26, _T("Зміни"), std::make_shared<CQaParamBinder>(change, false));
	ins_helper.addBinder(27, _T("Закр."), std::make_shared<CQaParamBinder>(close, false));
	ins_helper.addBinder(28, _T("Звільн."), std::make_shared<CQaParamBinder>(zv, false));
	ins_helper.addBinder(29, _T("Мін."), std::make_shared<CQaParamBinder>(min_penalty, false));
	ins_helper.addBinder(30, _T("Найм. сув."), std::make_shared<CQaParamBinder>(nm_suv, false));
	ins_helper.addBinder(31, _T("Звільн. крим."), std::make_shared<CQaParamBinder>(zv_kr, false));
	ins_helper.addBinder(32, _T("Без зм. 1 інст."), std::make_shared<CQaParamBinder>(no_ch_Ist, false));

	ins_helper.addBinder(33, _T("Коеф."), \
						std::make_shared<CQaKoefBinder>(Koef, false));
	ins_helper.addBinder(34, _T("Дата реєстр. в ДКС"), \
						std::make_shared<UIDateInsertBinder>(act_date, false));
	ins_helper.addBinder(35, _T("Перевірив"), \
				std::make_shared<CDbComboBoxInsertBinder>(checker, false, false));
	
	ins_helper.addBinder(36, _T("Витрати поштові"), \
						std::make_shared<CFeeBinder>(outg_post, false, false));
	ins_helper.addBinder(37, _T("Витрати добові"), \
						std::make_shared<CFeeBinder>(outg_daynight, false, false));

	std::string query = "INSERT INTO payments VALUES(";
	ins_helper.buildQuery(query);
	query += ')';

	stmt = conn->PrepareQuery(query.c_str());
}

bool CPaymentsInserter::insert() {

	Tstring error_str;
	if (!ins_helper.bind(stmt, 0, error_str)) {
		ErrorBox(error_str.c_str());
		return false;
	}

	try {
		stmt->execScalar();
	}
	catch (CDbException &e) {

		if (e.GetErrorCode() == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) {
			
			error_str = _T("Така стадія уже існує в цьому дорученні: ");
			error_str += stage->GetLabel();
			ErrorBox(error_str.c_str());
			return false;
		}
		else throw;
	}

	return true;
}

void CPaymentsInserter::Dispose() {

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
	if (lang && !lang->IsCreated()) delete lang;
	lang = nullptr;
	if (inv && !inv->IsCreated()) delete inv;
	inv = nullptr;
	if (age && !age->IsCreated()) delete age;
	age = nullptr;

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

CPaymentsInserter::~CPaymentsInserter() {

	Dispose();
}
