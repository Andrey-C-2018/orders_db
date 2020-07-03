#include <UnitTest++/UnitTest++.h>
#include <basic/ImmutableString.h>
#include <db_ext/SortingManager.h>
#include <db_ext/FilteringManager.h>
#include <db_ext/QueryModifier.h>

class CNullBinder : public IBinder {
public:
	size_t bind(std::shared_ptr<IDbBindingTarget> binding_target, \
				const size_t param_no) override { return 0;	}

	size_t affectedParamsCount() const override { return 0; }
	void reset() override { }
	
	virtual ~CNullBinder() { }
};

class QueryModifierTest final {
	enum {
		COLUMNS_COUNT = 10
	};
	std::string q;

	CSortingManager mgr_srt;
	CFilteringManager mgr_flt;
	CQueryModifier modifier;

public:
	QueryModifierTest() : q("SELECT * FROM order"), \
							mgr_srt(COLUMNS_COUNT), \
							modifier(q, q.size()) { }

	inline const char *getQuery() const {

		return modifier.getQuery().c_str();
	}

	void sortByColumn(const size_t column) {

		mgr_srt.sortByColumn(column);
		auto ordering_str = mgr_srt.buildOrderingString();
		modifier.changeOrdering(ordering_str);
	}

	void addFiltering(const char *expr) {

		size_t sz = strlen(expr);
		auto id = mgr_flt.addExpr(ImmutableString<char>(expr, sz), \
									std::make_shared<CNullBinder>());
		mgr_flt.enableExpr(id);

		auto filtering_str = mgr_flt.buildFilteringString();
		modifier.changeWherePart(filtering_str);
	}

	void resetOrdering() {

		mgr_srt.reset();
		modifier.resetOrdering();
	}

	void resetFiltering() {

		modifier.resetWherePart();
	}

};

SUITE(QueryModifierTest_tests) {

	TEST(CtorQueryTextOnly) {

		try {
			CQueryModifier m("SELECT * FROM order");
		}
		catch (CQueryModifierException &e) {
			CHECK_EQUAL(e.GetErrorCode(), \
						CQueryModifierException::E_NO_WHERE_STMT_MARK);
		}
		CQueryModifier m;
		m.Init("SELECT * FROM order ####");

		const char p[] = "id = 1";
		ImmutableString<char> str(p, sizeof(p) - 1);
		m.changeWherePart(str);

		const auto &q = m.getQuery();
		CHECK_EQUAL("SELECT * FROM order WHERE id = 1", q.c_str());
	}

	TEST(CtorQueryTextOnlyWithOrderBy) {

		CQueryModifier m;
		m.Init("SELECT * FROM order #### ORDER BY 1, 3");

		const char p[] = "id = 1";
		ImmutableString<char> str(p, sizeof(p) - 1);
		m.changeWherePart(str);

		const auto &q = m.getQuery();
		CHECK_EQUAL("SELECT * FROM order WHERE id = 1 ORDER BY 1, 3", q.c_str());

		const char p2[] = "1";
		ImmutableString<char> str2(p2, sizeof(p2) - 1);
		m.changeOrdering(str2);
		CHECK_EQUAL("SELECT * FROM order WHERE id = 1 ORDER BY 1", q.c_str());
	}

	TEST(OrderingPartChanged) {

		QueryModifierTest m;
		m.sortByColumn(1);
		CHECK_EQUAL("SELECT * FROM order ORDER BY 1", m.getQuery());
	}

	TEST(WherePartChanged) {

		QueryModifierTest m;
		m.addFiltering("id = ?");
		CHECK_EQUAL("SELECT * FROM order WHERE (? OR (id = ?))", \
					m.getQuery());
	}

	TEST(BothPartsChanged) {

		QueryModifierTest m;
		m.sortByColumn(1);
		m.addFiltering("id = ?");
		CHECK_EQUAL("SELECT * FROM order WHERE (? OR (id = ?)) ORDER BY 1", \
						m.getQuery());
	}

	TEST(BothPartsChangedSeveralTimes) {

		QueryModifierTest m;
		m.sortByColumn(2);
		m.addFiltering("id = ?");
		CHECK_EQUAL("SELECT * FROM order WHERE (? OR (id = ?)) ORDER BY 2", \
						m.getQuery());

		m.addFiltering("id_order_type = ?");
		CHECK_EQUAL("SELECT * FROM order WHERE (? OR (id = ?)) AND (? OR (id_order_type = ?)) ORDER BY 2", \
						m.getQuery());

		m.resetOrdering();
		CHECK_EQUAL("SELECT * FROM order WHERE (? OR (id = ?)) AND (? OR (id_order_type = ?))", \
					m.getQuery());

		m.resetFiltering();
		CHECK_EQUAL("SELECT * FROM order", m.getQuery());
	}

	TEST(BothPartsChangedSeveralTimesDiffOrder) {

		QueryModifierTest m;
		m.addFiltering("id = ?");
		CHECK_EQUAL("SELECT * FROM order WHERE (? OR (id = ?))", \
			m.getQuery());

		m.sortByColumn(2);
		m.addFiltering("id_order_type = ?");
		CHECK_EQUAL("SELECT * FROM order WHERE (? OR (id = ?)) AND (? OR (id_order_type = ?)) ORDER BY 2", \
			m.getQuery());

		m.resetOrdering();
		CHECK_EQUAL("SELECT * FROM order WHERE (? OR (id = ?)) AND (? OR (id_order_type = ?))", \
			m.getQuery());

		m.resetFiltering();
		CHECK_EQUAL("SELECT * FROM order", m.getQuery());
	}
}