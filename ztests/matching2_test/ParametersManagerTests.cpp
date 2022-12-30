#include <memory>
#include <basic/tstream.h>
#include <basic/Exception.h>
#include <basic/PropertiesFile.h>
#include <date/Date.h>
#include <UnitTest++/UnitTest++.h>
#include <ParametersManager.h>

const char PATH_TO_TEST_FILE[] = "config.ini";

class CParamsManagerTest {
	bool initialized;
	CPropertiesFile props;
	std::unique_ptr<CParametersManager> manager;

	bool processExceptions(bool(CParamsManagerTest::*func)()) noexcept {

		bool result = false;
		try {
			result = (this->*func)();
		}
		catch (XException &e) {
			Tcerr << e.what() << std::endl;
		}
		catch (std::exception &e) {
			Tcerr << e.what() << std::endl;
		}
		catch (...) {
			Tcerr << _T("Undefined exception\n");
		}

		return result;
	}

	bool init() {

		props.open(PATH_TO_TEST_FILE);
		manager.reset(new CParametersManager(&props));
		return true;
	}
public:
	CParamsManagerTest() noexcept : initialized(false) {

		initialized = processExceptions(&CParamsManagerTest::init);
	}

	inline bool testCopyCtorAndRvalue() {

		try {
			*manager = CParametersManager(&props);
		}
		catch (XException &e) {
			Tcerr << e.what() << std::endl;
			return false;
		}
		return processExceptions(&CParamsManagerTest::testCopyCtorAndRvalue);
	}

	inline bool isInitialized() const {	return initialized;	}

	virtual ~CParamsManagerTest() { }
};

SUITE(CParametersManager_tests) {

	TEST_FIXTURE(CParamsManagerTest, ctor) {

		CHECK(isInitialized());
	}
}