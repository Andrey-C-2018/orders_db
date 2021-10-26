#include <iostream>
#include <UnitTest++/UnitTest++.h>
#include <dbf3/DbfTable.h>

const char *FIELD_NAMES[] = {"ID", "NAME", "FEE", "MARK", "PDATE"};
const char FIELD_TYPES[] = {'N', 'C', 'N', 'L', 'D'};
const char *VALUES[][4] = { {"1", "a", "10.46", "20200524"}, \
							{"2", "bc", "0.28", "19981106"}, \
							{"4", "ff234", "0.00", "20031230"}, \
							{"10", "", "0.00", "19641018"}, \
							{"215",	"+-", "0.71", "19641019"}};
const bool VALUES_bool[] = {false, false, false, true, true};

const char *FIELD_NAMES2[] = {"OP", "ND", "NPD", "DA", "NREE", \
							"NLSA", "RRAX", "S", "KV", "KOBL", \
							"KRAJ", "KSEL", "KPK", "KFK", "KVK", \
							"KEKV", "DATERZ", "NPRZ", "S1", "KEKV1", \
							"NPRZ1", "S2", "KEKV2", "NPRZ2", "S3", \
							"KEKV3", "NPRZ3", "S4", "PRM", "PRM1", \
							"PRM2", "NAZB", "MFO", "NLSK", "KOKPO", \
							"POLUT", "SK", "SKD", "VZK", "KPOL", \
							"NAZ", "NDOC", "DATEDOC", "KEKD", "MFOP", \
							"KOKPOR", "NAMER", "NDOCU", "DATEDOCU", "IBANA", \
							"IBANK"};

SUITE(DbfTable_tests) {
	
	TEST(ctor) {
		
		DbfTable t;
		CHECK_EQUAL((size_t)-1, t.getCurrentRec());
		
		t.close();
		CHECK_EQUAL((size_t)-1, t.getCurrentRec());
	}

	TEST(open) {
		
		DbfTable t;
		try {
			
			t.open("test.dbf");
			CHECK(t.isOpen());
			
			CHECK_EQUAL(5, t.getFieldsCount());
			CHECK_EQUAL(5, t.getRecordsCount());
			CHECK_EQUAL((size_t)-1, t.getCurrentRec());
			
			for(size_t i = 0; i < t.getFieldsCount(); ++i) {
				CHECK_EQUAL(FIELD_NAMES[i], t.getFieldName(i));
				CHECK_EQUAL(FIELD_TYPES[i], t.getFieldType(i));
			}
			
			for (size_t i = 0; i < t.getRecordsCount(); ++i) {
				t.fetchNext();
				CHECK_EQUAL(i, t.getCurrentRec());
				CHECK_EQUAL(VALUES[i][0], t.getNumericCellValue(0));
				CHECK_EQUAL(VALUES[i][1], t.getStrCellValue(1));
				CHECK_EQUAL(VALUES[i][2], t.getNumericCellValue(2));
				CHECK_EQUAL(VALUES_bool[i], t.getBooleanCellValue(3));
				CHECK_EQUAL(VALUES[i][3], t.getDateCellValue(4));
			}
		}
		catch(XException &e) {
			
			std::cerr << e.what() << std::endl;
			CHECK(1 == 0);
		}
	}

	TEST(open2) {

		DbfTable t;
		try {
			t.open("test2.DBF");
			CHECK(t.isOpen());

			CHECK_EQUAL(51, t.getFieldsCount());
			CHECK_EQUAL(5, t.getRecordsCount());
			CHECK_EQUAL((size_t) -1, t.getCurrentRec());

			for (size_t i = 0; i < t.getFieldsCount(); ++i)
				CHECK_EQUAL(FIELD_NAMES2[i], t.getFieldName(i));
		}
		catch (XException &e) {

			std::cerr << e.what() << std::endl;
			CHECK(1 == 0);
		}
	}

	TEST(open2_no_records) {

		DbfTable t;
		try {
			t.open("test2_norec.DBF");
			CHECK(t.isOpen());

			CHECK_EQUAL(51, t.getFieldsCount());
			CHECK_EQUAL(0, t.getRecordsCount());
			CHECK_EQUAL((size_t) -1, t.getCurrentRec());
		}
		catch (XException &e) {

			std::cerr << e.what() << std::endl;
					CHECK(1 == 0);
		}
	}
}