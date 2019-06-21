#include <UnitTest++/UnitTest++.h>
#include <basic/Exception.h>

SUITE(XExceptionTests) {

	TEST(EmptyException) {

		try {
			XException e(0, "");
		}
		catch (XException &e) {

			CHECK(e.GetErrorCode() == 0);

			auto err_descr = e.what();
			CHECK(err_descr && err_descr[0] == 0);
		}

	}

	TEST(EncodingIndependence) {

		XException e(1, "Test");
		XException e_UCS16(2, L"Test");

		auto err_descr1 = e.what();
		auto err_descr2 = e_UCS16.what();
		CHECK(!Tstrcmp(err_descr1, err_descr2));
	}
}