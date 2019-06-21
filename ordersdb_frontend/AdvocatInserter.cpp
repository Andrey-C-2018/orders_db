#include <assert.h>
#include <basic/tstring.h>
#include <db/IDbConnection.h>
#include "AdvocatInserter.h"

const char fields_order[12][15] = { "id_advocat", "adv_name", "adv_name_short", \
								"license_no", "license_date", "id_exm", \
								"post_index", "address", "adv_bdate", \
								"id_main_distr", "org_name", "org_type" };

//**************************************************************

class CAdvShortNameBinder : public IBinder {
	const Tchar *adv_name_short;
public:
	CAdvShortNameBinder() : adv_name_short(nullptr) { }

	inline void setAdv_nameShort(const Tchar *adv_name_short) {

		this->adv_name_short = adv_name_short;
	}

	void bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override {

		assert(adv_name_short);
		binding_target->bindValue(param_no, adv_name_short);
	}

	virtual ~CAdvShortNameBinder() { }
};

//**************************************************************

CAdvocatInserter::CAdvocatInserter() { }

void CAdvocatInserter::addInsertionBinder(const char *field_name, \
											std::shared_ptr<IBinder> binder) {

	assert(initialized_fields_count < FIELDS_MAX_COUNT);
	assert(field_name);
	
	auto p = std::find(std::begin(fields_order), std::end(fields_order), field_name);
	assert(p != std::end(fields_order));

	auto order = std::distance(std::begin(fields_order), p);
	fields[order].name = field_name;
	fields[order].binder = binder;

	++initialized_fields_count;
}

void CAdvocatInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	assert(initialized_fields_count == FIELDS_MAX_COUNT - 1);

	addInsertionBinder("adv_name_short", std::make_shared<CAdvShortNameBinder>());
	assert(initialized_fields_count == FIELDS_MAX_COUNT);

	stmt = conn->PrepareQuery("INSERT INTO advocats VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
}

CAdvocatInserter::~CAdvocatInserter() { }
