#include <basic/XConv.h>
#include <db_ext/DbTable.h>
#include <xwindows/XWidget.h>
#include <db_controls/DbComboBox.h>
#include "PaymentsInserter.h"

enum {
	MAX_INT_FIELD_LEN = getDigitsCountOfType<int>() + 1
};

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

CPaymentsInserter::CPaymentsInserter() : CDbInserter("payments", FIELDS_COUNT), \
				stage(nullptr), informer(nullptr), cycle(nullptr), article(nullptr), \
				fee(nullptr), outgoings(nullptr), id_act(nullptr), act_date(nullptr), \
				act_reg_date(nullptr), age(nullptr), inv(nullptr), lang(nullptr), ill(nullptr), \
				zek(nullptr), vpr(nullptr), reduce(nullptr), change(nullptr), \
				close(nullptr), zv(nullptr), min_penalty(nullptr), nm_suv(nullptr), \
				zv_kr(nullptr), no_ch_Ist(nullptr), Koef(nullptr), checker(nullptr) { }

void CPaymentsInserter::getCurrRecord(std::shared_ptr<CDbTable> db_table) {
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
	size_t field_no = meta_info.getFieldIndexByName("cycle");
	cycle->SetLabel(getIntFieldAsString(rs, field_no, buffer));

	setStrWidgetLabel(article, rs, meta_info, "article");
	setStrWidgetLabel(fee, rs, meta_info, "fee");
	setStrWidgetLabel(outgoings, rs, meta_info, "outgoings");

	informer->SetCurrRecord(rs, meta_info.getFieldIndexByName("id_informer"), INT_TYPE_HINT);

	setStrWidgetLabel(id_act, rs, meta_info, "id_act");
	setDateWidgetLabel(act_date, rs, meta_info, "act_date", buffer);
}

void CPaymentsInserter::prepare(std::shared_ptr<IDbConnection> conn) {

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

CPaymentsInserter::~CPaymentsInserter() { }
