#include "ParametersManager.h"
#include <basic/PropertiesFile.h>

inline bool getParamValue(CPropertiesFile *props, const Tchar *prop_name, \
							const bool old_value, Tstring &buffer) {

	bool not_found;
	int i_value = props->getIntProperty(prop_name, buffer, not_found);
	return (not_found && old_value) || (!not_found && (i_value == 1));
}

CParametersManager::CParametersManager(CPropertiesFile *props) : \
								full_matching(false), only_unpaid_acts(true), \
								check_processed(true), cached_acts_names(false) {
	
	assert(props);
	Tstring buffer;
		
	auto str_date = props->getStringProperty(_T("initial_acts_date"), buffer);
	if (!str_date)
		throw XException(0, \
			_T("Параметр початкової дати 'initial_acts_date' відсутній у config.ini"));

	if (!initial_acts_date.setDateGerman(str_date)) {
		XException e(0, _T("Невірний формат початкової дати: "));
		e << str_date;
		throw e;
	}
	if (initial_acts_date > CDate::now()) {
		XException e(0, _T("Початкова дата в майбутньому: "));
		e << str_date;
		throw e;
	}
	initial_acts_date.toStringSQL(date_buffer);

	full_matching = getParamValue(props, _T("full_matching"), full_matching, buffer);

	if (!full_matching) {
		only_unpaid_acts = getParamValue(props, _T("only_unpaid_acts"), \
											only_unpaid_acts, buffer);
		check_processed = getParamValue(props, _T("check_processed"), \
											check_processed, buffer);
		cached_acts_names = getParamValue(props, _T("cached_acts_names"), \
											cached_acts_names, buffer);
	}
}

CParametersManager::~CParametersManager() { }
