#pragma once
#include <memory>

struct IDbConnection;
class XWindow;
class CDbGrid;
class CDbTable;

class CAdvocatsListManager {
	std::shared_ptr<CDbTable> db_table;
	CDbGrid *grid;
	std::shared_ptr<CDbTable> createDbTable(std::shared_ptr<IDbConnection> conn);

public:
	CAdvocatsListManager();

	void Init(std::shared_ptr<IDbConnection> conn_,
				XWindow *parent);
	void CreateWidgets(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height);

	virtual ~CAdvocatsListManager();
};

