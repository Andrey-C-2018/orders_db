#include <basic/XConv.h>
#include <db_ext/DbTable.h>
#include <xwindows/XWidget.h>
#include <db_controls/DbComboBox.h>
#include "PaymentsInserter.h"

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
			params.error_str += _T("� ��������� ������� ���䳿\n");
			return true;
		}

		bool is_null;
		int v = rs->getInt(order_params[0], is_null);
		assert(is_null);
		binding_target->bindValue(params.param_no - 3, v);
		
		v = rs->getInt(order_params[1], is_null);
		assert(is_null);
		binding_target->bindValue(params.param_no - 2, v);

		CDate dt = rs->getDate(order_params[2], is_null);
		assert(is_null);
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

		auto p = Tstrchr(fee_str, _T('.'));
		if (p) size -= (p - fee_str);
		if (size > 6) {
			++params.param_no;
			params.error_str += _T("���� ���� ��� ������� �� ������ ������������ 1 ���. ���.\n");
			return true;
		}

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
			params.error_str += _T("��'� ���� �� ���� ���� ������ �� ");
			Tchar buffer[3];
			params.error_str += XConv::ToString((size_t)MAX_ACT_NAME_LEN, buffer);
			params.error_str += _T(" ����.\n");
			return true;
		}
		act_name.clear();
		act_name.reserve(size);

		for (size_t i = 0; i < size; ++i) {
			Tchar ch = ToUpper(act_str[i]);

			if (ch == _T(' ')) continue;

			if (!((ch >= _T('0') && ch <= _T('9')) || \
				(ch >= _T('�') && ch <= _T('�')) || \
				ch == _T('.'))) {

				++params.param_no;
				params.error_str += _T("������ ��'� ����\n");
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

class CActDateBinder : public UIDateInsertBinder {
	enum { INVALID_FIELD_INDEX = (size_t)-1 };

	std::shared_ptr<CDbTable> db_table;
	XWidget *act_reg_date_widget;
	size_t order_date_no;
public:
	CActDateBinder(std::shared_ptr<CDbTable> db_table_, \
					XWidget *act_date_holder, bool free_widget, \
					XWidget *act_reg_date_holder) : \
						UIDateInsertBinder(act_date_holder, free_widget), \
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
			params.error_str += _T(": ������� ������ ����\n");
			++params.param_no;
			return true;
		}
		if (date > CDate::now()) {
			int option = _plMessageBoxYesNo(_T("���� ���� � �����������. ������������?"));
			if (option == IDNO) return false;
		}
		
		auto rs = db_table->getResultSet();
		bool is_null;
		CDate order_date = rs->getDate(order_date_no, is_null);
		assert(!is_null);
		if (date < order_date) {
			params.error_str += field_name;
			params.error_str += _T(": ���� ���� �� ���� ���� ������ �� ���� ���������\n");
			++params.param_no;
			return true;
		}
		if (date == order_date) {
			int option = _plMessageBoxYesNo(_T("���� ���� ������� �� ����� ���������.\n ����������, �� ��� ��� ��������� � ��� �� ����.\n ������������?"));
			if (option == IDNO) return false;
		}

		CDate act_reg_date(act_reg_date_widget->GetLabel(), CDate::GERMAN_FORMAT);
		if (!act_reg_date.isNull() && date < act_reg_date) {
			params.error_str += field_name;
			params.error_str += _T(": ���� ���� �� ���� ���� ������ �� ���� ��������� ����\n");
			++params.param_no;
			return true;
		}

		binding_target->bindValue(params.param_no, date);
		++params.param_no;
		return true;
	}

	virtual ~CActDateBinder() { }
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
		ErrorBox(_T("������� �������. ��������� ������� �������� �����"));
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

	addBinder(0, _T("�������� ���������"), std::make_shared<COrderParamsBinder>(db_table));
	addBinder(3, _T("����"), std::make_shared<CFeeBinder>(fee, false));
	addBinder(4, _T("�����"), std::make_shared<CDbComboBoxInsertBinder>(stage, false));
	addBinder(5, _T("������"), std::make_shared<UITextInsertBinder>(article, false));
	addBinder(6, _T("����������"), std::make_shared<CDbComboBoxInsertBinder>(informer, false));
	addBinder(7, _T("���"), std::make_shared<CActNameBinder>(id_act, false));
	addBinder(8, _T("���� ����"), std::make_shared<CActDateBinder>(db_table, act_date, false, act_reg_date));
	addBinder(9, _T("����"), std::make_shared<UIIntInsertBinder>(cycle, false));

	CDbInserter::prepare(conn);
}

void CPaymentsInserter::insert() {

	CDbInserter::insert();
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