#include "ParametersManager.h"
#include <basic/PropertiesFile.h>

CParametersManager::CParametersManager() : default_center(-1) { }

void CParametersManager::Init(CPropertiesFile *props) {
	
	assert(props);
	Tstring buffer;
		
	auto str_date = props->getStringProperty(_T("initial_order_date"), buffer);
	if (!str_date)
		throw XException(0, \
			_T("Параметр початкової дати 'initial_order_date' відсутній у config.ini"));

	if (!initial_order_date.setDateGerman(str_date)) {
		XException e(0, _T("Невірний формат початкової дати: "));
		e << str_date;
		throw e;
	}
	if (initial_order_date > CDate::now()) {
		XException e(0, _T("Початкова дата в майбутньому: "));
		e << str_date;
		throw e;
	}
	initial_order_date.toStringSQL(date_buffer);

	bool not_found;
	default_center = props->getIntProperty(_T("default_center"), buffer, not_found);
	if(not_found)
		throw XException(0, \
			_T("Параметр центру 'default_center' відсутній у config.ini"));

	if (default_center < 1 || default_center > 5) {
		XException e(0, _T("Невірний номер центру: "));
		e << default_center;
		throw e;
	}
}

CParametersManager::~CParametersManager() { }
