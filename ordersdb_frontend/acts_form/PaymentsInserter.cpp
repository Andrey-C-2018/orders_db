#include <basic/XConv.h>
#include <db/DbException.h>
#include <db_ext/DbTable.h>
#include <xwindows/XWidget.h>
#include <db_controls/DbComboBox.h>
#include "PaymentsInserter.h"
#include "ParametersManager.h"

enum {
	MAX_INT_FIELD_LEN = getDigitsCountOfType<int>() + 1
};

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

		params.param_no += 3;
		auto rs = db_table->getResultSet();
		if (rs->empty()) {
			params.error_str += _T("У дорученні відсутні стадії\n");
			return true;
		}

		bool is_null;
		int v = rs->getInt(order_params[0], is_null);
		assert(!is_null);
		binding_target->bindValue(params.param_no - 3, v);
		
		v = rs->getInt(order_params[1], is_null);
		assert(!is_null);
		binding_target->bindValue(params.param_no - 2, v);

		CDate dt = rs->getDate(order_params[2], is_null);
		assert(!is_null);
		binding_target->bindValue(params.param_no - 1, dt);
		
		return true;
	}

	virtual ~COrderParamsBinder() { }
};

class CFeeBinder : public CVisualInsertBinder {
public:
	CFeeBinder(XWidget *fee_holder, bool free_widget) : \
				CVisualInsertBinder(fee_holder, free_widget) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		size_t size;
		auto fee_str = widget->GetLabel(size);

		if (!size) {
			++params.param_no;
			params.error_str += field_name;
			params.error_str += _T(": не може бути порожнім\n");
			return true;
		}

		size_t i = 0;
		while (i < size && \
			((fee_str[i] >= _T('0') && fee_str[i] <= _T('9')) || \
				fee_str[i] == _T('.') || fee_str[i] == _T(','))) ++i;

		if (i < size) {
			++params.param_no;
			params.error_str += field_name;
			params.error_str += _T(": невірний формат: ");
			params.error_str += fee_str;
			params.error_str += _T('\n');
			return true;
		}

		auto p = Tstrchr(fee_str, _T('.'));
		const Tchar *p2 = nullptr;
		if (!p) {
			p2 = Tstrchr(fee_str, _T(','));
			p = p2;
		}

		if (p) size = (p - fee_str);
		if (size > 6) {
			++params.param_no;
			params.error_str += field_name;
			params.error_str += _T(": не може перевищувати 1 млн. грн.\n");
			return true;
		}

		if (p2) {
			Tchar fee[10];
			Tstrncpy(fee, fee_str, 6);
			size_t fee_size = Tstrlen(fee);
			fee[fee_size] = _T('.');
			++fee_size;
			Tstrncpy(fee + fee_size, fee_str + fee_size, 2);
			fee[fee_size + 2] = _T('\0');
			binding_target->bindValue(params.param_no, fee);
		}
		else
			binding_target->bindValue(params.param_no, fee_str);
		++params.param_no;
		return true;
	}

	virtual ~CFeeBinder() { }
};

class CActNameBinder : public CVisualInsertBinder {
	enum {MAX_ACT_NAME_LEN = 25};

	Tstring act_name;
public:
	CActNameBinder(XWidget *act_name_holder, bool free_widget) : \
					CVisualInsertBinder(act_name_holder, free_widget) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		size_t size;
		auto act_str = widget->GetLabel(size);
		if (size > MAX_ACT_NAME_LEN) {
			++params.param_no;
			params.error_str += _T("Ім'я акта не може бути довшим за ");
			Tchar buffer[3];
			params.error_str += XConv::ToString((size_t)MAX_ACT_NAME_LEN, buffer);
			params.error_str += _T(" симв.\n");
			return true;
		}
		act_name.clear();
		act_name.reserve(size);

		for (size_t i = 0; i < size; ++i) {
			Tchar ch = ToUpper(act_str[i]);

			if (ch == _T(' ')) continue;

			if (!((ch >= _T('0') && ch <= _T('9')) || \
				(ch >= _T('А') && ch <= _T('Я')) || \
				ch == _T('.') || ch == _T('І'))) {

				++params.param_no;
				params.error_str += _T("Невірне ім'я акта: ");
				params.error_str += act_str;
				params.error_str += _T('\n');
				return true;
			}
			act_name += ch;
		}

		if(act_name.empty())
			binding_target->bindValue(params.param_no, _T(""));
		else
			binding_target->bindValue(params.param_no, act_name.c_str());
		++params.param_no;
		return true;
	}

	virtual ~CActNameBinder() { }
};

class CActDateBinder : public CVisualInsertBinder {
	enum { INVALID_FIELD_INDEX = (size_t)-1 };

	std::shared_ptr<CDbTable> db_table;
	XWidget *act_reg_date_widget;
	size_t order_date_no;
public:
	CActDateBinder(std::shared_ptr<CDbTable> db_table_, \
					XWidget *act_date_holder, bool free_widget, \
					XWidget *act_reg_date_holder) : \
						CVisualInsertBinder(act_date_holder, free_widget), \
						db_table(db_table_), \
						act_reg_date_widget(act_reg_date_holder), \
						order_date_no(INVALID_FIELD_INDEX) {
	
		assert(act_reg_date_widget);
		const CMetaInfo &meta_info = db_table->getQuery().getMetaInfo();
		order_date_no = meta_info.getFieldIndexByName("order_date");
	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		size_t size;
		auto date_str = widget->GetLabel(size);
		if (size == 0) {
			++params.param_no;
			binding_target->bindNull(params.param_no);
			return true;
		}

		CDate date(date_str, CDate::GERMAN_FORMAT);
		if (date.isNull()) {

			params.error_str += field_name;
			params.error_str += _T(": невірний формат дати\n");
			++params.param_no;
			return true;
		}
		if (date > CDate::now()) {
			int option = _plMessageBoxYesNo(_T("Дата акта в майбутньому. Продовжувати?"));
			if (option == IDNO) return false;
		}
		
		auto rs = db_table->getResultSet();
		bool is_null;
		CDate order_date = rs->getDate(order_date_no, is_null);
		assert(!is_null);
		if (date < order_date) {
			params.error_str += field_name;
			params.error_str += _T(": не може бути меншою за дату доручення\n");
			++params.param_no;
			return true;
		}
		if (date == order_date) {
			int option = _plMessageBoxYesNo(_T("Дата акта співпадає із датою доручення.\n Малоімовірно, що акт був прийнятий в той же день.\n Продовжувати?"));
			if (option == IDNO) return false;
		}

		CDate act_reg_date(act_reg_date_widget->GetLabel(), CDate::GERMAN_FORMAT);
		if (!act_reg_date.isNull() && date < act_reg_date) {
			params.error_str += field_name;
			params.error_str += _T(": не може бути меншою за дату прийняття акта\n");
			++params.param_no;
			return true;
		}

		binding_target->bindValue(params.param_no, date);
		++params.param_no;
		return true;
	}

	virtual ~CActDateBinder() { }
};

class CActRegDateBinder : public CVisualInsertBinder {
	enum { INVALID_FIELD_INDEX = (size_t)-1 };

	std::shared_ptr<CDbTable> db_table;
	size_t order_date_no;
public:
	CActRegDateBinder(std::shared_ptr<CDbTable> db_table_, \
						XWidget *act_date_holder, bool free_widget) : \
						CVisualInsertBinder(act_date_holder, free_widget), \
					db_table(db_table_), \
					order_date_no(INVALID_FIELD_INDEX) {

		const CMetaInfo &meta_info = db_table->getQuery().getMetaInfo();
		order_date_no = meta_info.getFieldIndexByName("order_date");
	}

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		size_t size;
		auto date_str = widget->GetLabel(size);
		if (size == 0) {
			++params.param_no;
			binding_target->bindNull(params.param_no);
			return true;
		}

		CDate date(date_str, CDate::GERMAN_FORMAT);
		if (date.isNull()) {

			params.error_str += field_name;
			params.error_str += _T(": невірний формат дати\n");
			++params.param_no;
			return true;
		}
		if (date > CDate::now()) {
			int option = _plMessageBoxYesNo(_T("Дата прийняття акта в майбутньому. Продовжувати?"));
			if (option == IDNO) return false;
		}

		auto rs = db_table->getResultSet();
		bool is_null;
		CDate order_date = rs->getDate(order_date_no, is_null);
		assert(!is_null);
		if (date < order_date) {
			params.error_str += field_name;
			params.error_str += _T(": не може бути меншою за дату доручення\n");
			++params.param_no;
			return true;
		}
		if (date == order_date) {
			int option = _plMessageBoxYesNo(_T("Дата прийняття акта співпадає із датою доручення. \nПродовжувати?"));
			if (option == IDNO) return false;
		}

		binding_target->bindValue(params.param_no, date);
		++params.param_no;
		return true;
	}

	virtual ~CActRegDateBinder() { }
};

class CQaParamBinder : public CVisualInsertBinder {
public:
	CQaParamBinder(XWidget *qa_param_holder, bool free_widget) : \
					CVisualInsertBinder(qa_param_holder, free_widget) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		size_t size;
		auto qa_param_str = widget->GetLabel(size);
		if (size == 0) {
			params.error_str += field_name;
			params.error_str += _T(": не може бути порожнім\n");
			++params.param_no;
			return true;
		}

		if (qa_param_str[0] != _T('0') && qa_param_str[0] != _T('1')) {
			params.error_str += field_name;
			params.error_str += _T(": допустимі значення - 0 або 1\n");
			++params.param_no;
			return true;
		}

		binding_target->bindValue(params.param_no, (int)(qa_param_str[0] - _T('0')));
		++params.param_no;
		return true;
	}

	virtual ~CQaParamBinder() { }
};

class CQaKoefBinder : public CVisualInsertBinder {
public:
	CQaKoefBinder(XWidget *qa_koef_holder, bool free_widget) : \
					CVisualInsertBinder(qa_koef_holder, free_widget) { }

	bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				Params &params, const Tchar *field_name) override {

		size_t size;
		auto qa_koef_str = widget->GetLabel(size);
		if (size == 0) {
			params.error_str += field_name;
			params.error_str += _T(": не може бути порожнім\n");
			++params.param_no;
			return true;
		}

		Tchar *end = nullptr;
		float value = wcstof(qa_koef_str, &end);
		
		if (end - qa_koef_str > 4 || \
			!(value <= 1e-6 || (value - 0.25F) <= 1e-6 || \
			(value - 0.5F) <= 1e-6 || (value - 0.75F) <= 1e-6 || \
			(value - 1.0F) <= 1e-6)) {

			params.error_str += field_name;
			params.error_str += _T(": допустимі значення - 0, 0.25, 0.5, 0.75 або 1\n");
			++params.param_no;
			return true;
		}

		binding_target->bindValue(params.param_no, qa_koef_str);
		++params.param_no;
		return true;
	}

	virtual ~CQaKoefBinder() { }
};

//*****************************************************

inline const Tchar *getIntFieldAsString(std::shared_ptr<const IDbResultSet> rs, \
										const size_t field_no, \
										Tchar(&buffer)[MAX_INT_FIELD_LEN]) {

	bool is_null;
	int value = rs->getInt(field_no, is_null);
	assert(!is_null);

	return XConv::ToString(value, buffer);
}

inline ImmutableString<char> getFieldAsString(std::shared_ptr<const IDbResultSet> rs, \
												const size_t field_no, \
												char type_hint) {

	return rs->getImmutableString(field_no);
}

inline ImmutableString<wchar_t> getFieldAsString(std::shared_ptr<const IDbResultSet> rs, \
													const size_t field_no, \
													wchar_t type_hint) {

	return rs->getImmutableWString(field_no);
}

inline void setIntWidgetLabel(XWidget *widget, std::shared_ptr<const IDbResultSet> rs, \
								const CMetaInfo &meta_info, const char *field_name, \
								Tchar(&buffer)[MAX_INT_FIELD_LEN]) {

	size_t field_no = meta_info.getFieldIndexByName(field_name);
	widget->SetLabel(getIntFieldAsString(rs, field_no, buffer));
}

inline void setStrWidgetLabel(XWidget *widget, std::shared_ptr<const IDbResultSet> rs, \
								const CMetaInfo &meta_info, const char *field_name) {

	size_t field_no = meta_info.getFieldIndexByName(field_name);
	auto label = getFieldAsString(rs, field_no, Tchar());
	if (label.isNull()) {
		widget->SetLabel(_T(""));
		return;
	}
	widget->SetLabel(label.str, label.size);
}

inline void setDateWidgetLabel(XWidget *widget, std::shared_ptr<const IDbResultSet> rs, \
								const CMetaInfo &meta_info, const char *field_name, \
								Tchar *buffer) {

	size_t field_no = meta_info.getFieldIndexByName(field_name);
	bool is_null;
	CDate dt = rs->getDate(field_no, is_null);

	if (is_null) {
		widget->SetLabel(_T(""));
		return;
	}
	dt.toStringGerman(buffer);
	widget->SetLabel(buffer);
}

//*****************************************************

CPaymentsInserter::CPaymentsInserter(std::shared_ptr<CDbTable> db_table_) : db_table(db_table_), \
				CDbInserter("payments", FIELDS_COUNT), \
				stage(nullptr), informer(nullptr), cycle(nullptr), article(nullptr), \
				fee(nullptr), outgoings(nullptr), id_act(nullptr), act_date(nullptr), \
				act_reg_date(nullptr), age(nullptr), inv(nullptr), lang(nullptr), ill(nullptr), \
				zek(nullptr), vpr(nullptr), reduce(nullptr), change(nullptr), \
				close(nullptr), zv(nullptr), min_penalty(nullptr), nm_suv(nullptr), \
				zv_kr(nullptr), no_ch_Ist(nullptr), Koef(nullptr), checker(nullptr) { }

void CPaymentsInserter::getCurrRecord() {
	enum {
		INT_TYPE_HINT = -1
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

	setStrWidgetLabel(article, rs, meta_info, "article");
	setStrWidgetLabel(fee, rs, meta_info, "fee");
	setStrWidgetLabel(outgoings, rs, meta_info, "outgoings");

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
	checker->SetCurrRecord(rs, meta_info.getFieldIndexByName("id_checker"), INT_TYPE_HINT);
}

void CPaymentsInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	assert(stage);
	assert(cycle);
	assert(article);
	assert(fee);
	assert(outgoings);
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

	addBinder(0, _T("Реквізити доручення"), std::make_shared<COrderParamsBinder>(db_table));
	addBinder(3, _T("Сума"), std::make_shared<CFeeBinder>(fee, false));
	addBinder(4, _T("Стадія"), std::make_shared<CDbComboBoxInsertBinder>(stage, false));
	addBinder(5, _T("Стаття"), std::make_shared<UITextInsertBinder>(article, false));
	addBinder(6, _T("Інформатор"), std::make_shared<CDbComboBoxInsertBinder>(informer, false));
	addBinder(7, _T("Акт"), std::make_shared<CActNameBinder>(id_act, false));
	addBinder(8, _T("Дата акта"), std::make_shared<CActDateBinder>(db_table, act_date, false, act_reg_date));
	addBinder(9, _T("Цикл"), std::make_shared<UIIntInsertBinder>(cycle, false));
	addBinder(10, _T("Користувач"), std::make_shared<CIntInsertBinder>(id_user));
	defStaticInsertion(11, "NOW()");
	defStaticInsertion(12, "NULL");
	defStaticInsertion(13, "NULL"); 
	addBinder(14, _T("Дата прийняття акта"), std::make_shared<CActRegDateBinder>(db_table, act_reg_date, false));
	defStaticInsertion(15, "NULL");
	addBinder(16, _T("Витрати"), std::make_shared<CFeeBinder>(outgoings, false));
	defStaticInsertion(17, "0.0");

	addBinder(18, _T("Вік"), std::make_shared<CQaParamBinder>(age, false));
	addBinder(19, _T("Вади"), std::make_shared<CQaParamBinder>(inv, false));
	addBinder(20, _T("Мова"), std::make_shared<CQaParamBinder>(lang, false));
	addBinder(21, _T("Хвороба"), std::make_shared<CQaParamBinder>(ill, false));
	addBinder(22, _T("ЗЕК"), std::make_shared<CQaParamBinder>(zek, false));
	addBinder(23, _T("Випр."), std::make_shared<CQaParamBinder>(vpr, false));
	addBinder(24, _T("Зменш."), std::make_shared<CQaParamBinder>(reduce, false));
	addBinder(25, _T("Зміни"), std::make_shared<CQaParamBinder>(change, false));
	addBinder(26, _T("Закр."), std::make_shared<CQaParamBinder>(close, false));
	addBinder(27, _T("Звільн."), std::make_shared<CQaParamBinder>(zv, false));
	addBinder(28, _T("Мін."), std::make_shared<CQaParamBinder>(min_penalty, false));
	addBinder(29, _T("Найм. сув."), std::make_shared<CQaParamBinder>(nm_suv, false));
	addBinder(30, _T("Звільн. крим."), std::make_shared<CQaParamBinder>(zv_kr, false));
	addBinder(31, _T("Без зм. 1 інст."), std::make_shared<CQaParamBinder>(no_ch_Ist, false));

	addBinder(32, _T("Коеф."), std::make_shared<CQaKoefBinder>(Koef, false));
	addBinder(33, _T("Дата реєстр. в ДКС"), std::make_shared<UIDateInsertBinder>(act_date, false));
	addBinder(34, _T("Перевірив"), std::make_shared<CDbComboBoxInsertBinder>(checker, false));
	
	CDbInserter::prepare(conn);
}

void CPaymentsInserter::insert() {

	try {
		CDbInserter::insert();
	}
	catch (CDbInserterException &e) {

		ErrorBox(e.what());
	}
	catch (CDbException &e) {

		if (e.GetErrorCode() == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) {
			Tstring error_str = _T("Така стадія уже існує в цьому дорученні: ");
			error_str += stage->GetLabel();
			ErrorBox(error_str.c_str());
		}
		else throw;
	}
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
