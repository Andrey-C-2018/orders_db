#include <map>
#include <iostream>
#include <basic/PropertiesFile.h>
#include <basic/TextConv.h>
#include <db/SQLite/SQLiteConnection.h>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include <db/IDbResultSetMetaData.h>
#include "mysql_initializer.h"
#include "DbTable.h"
#include "Matching.h"
#include "CurrencyDiffCalc.h"
#include "AdvActDistanceCalc.h"
#include "OutputWrapper.h"

struct CAdvocatBlock {
	size_t position;
	size_t size;
};

typedef std::map<std::string, CAdvocatBlock> CAdvocats;

std::shared_ptr<IDbConnection> createSQLiteConnection(const CPropertiesFile &props);
CAdvocats fillAdvocatBlocks(std::shared_ptr<const ITable> in, const size_t col_index);

int main() {

	setlocale(LC_ALL, "ukr_ukr.1251");

	try {
		CPropertiesFile props;
		props.open("config.ini");

		auto mysql_conn = createMySQLConnection(props);
		std::string query = "SELECT aa.id_center, aa.id_order, aa.order_date, aa.id_stage, aa.cycle,";
		query += " b.adv_name, aa.act_date, aa.fee as fee_DB, aa.id_act ";
		query += "FROM orders a INNER JOIN payments aa ON";
		query += " a.id_center_legalaid = aa.id_center AND a.id = aa.id_order AND a.order_date = aa.order_date";
		query += " INNER JOIN advocats b ON a.id_adv = b.id_advocat ";
		query += "WHERE a.zone = ? AND aa.act_date >= '2019-01-01' AND aa.is_paid IS NULL AND aa.fee <> 0 ";
		query += "ORDER BY adv_name, id_act";

		auto ordersdb_stmt = mysql_conn->PrepareQuery(query.c_str());
		ordersdb_stmt->bindValue(0, L"аж");
		auto ordersdb_result_set = ordersdb_stmt->exec();
		auto ordersdb_table = std::make_shared<CMySQLDbTable>(ordersdb_result_set, \
														ordersdb_stmt->getResultSetMetadata());

		auto sqlite_conn = createSQLiteConnection(props);
		query = "SELECT act_name, fee as fee_1C, adv_name as adv_name_1C, payment_date ";
		query += "FROM payments_1c ORDER BY adv_name, act_name";

		auto stmt_1c = sqlite_conn->PrepareQuery(query.c_str());
		auto result_set_1c = stmt_1c->exec();
		auto table_1c = std::make_shared<CMySQLDbTable>(result_set_1c, \
														stmt_1c->getResultSetMetadata());

		std::ofstream result;
		result.open("result.csv", std::ios::out | std::ios::trunc);
		if (!result.is_open())
			throw XException(0, "can't create the results file: results.csv");

		size_t adv_name_index_in1 = ordersdb_table->getColIndexByName("adv_name");
		if (adv_name_index_in1 == ITable::NOT_FOUND)
			throw XException(0, "the column 'adv_name' is not found in the table from DB");

		size_t adv_name_index_in2 = table_1c->getColIndexByName("adv_name_1C");
		if (adv_name_index_in2 == ITable::NOT_FOUND)
			throw XException(0, "the column 'adv_name' is not found in the table from 1C");

		auto blocks1 = fillAdvocatBlocks(ordersdb_table, adv_name_index_in1);
		auto blocks2 = fillAdvocatBlocks(table_1c, adv_name_index_in2);

		CEqualMatching matching(ordersdb_table, table_1c);

		std::shared_ptr<IDistanceCalculator> Acts_distance_calc = std::make_shared<CAdvActDistanceCalc>();
		matching.addDistanceCalculator("id_act", "act_name", Acts_distance_calc);

		std::shared_ptr<IDistanceCalculator> Currency_calc = std::make_shared<CCurrencyDiffCalc>();
		matching.addDistanceCalculator("fee_DB", "fee_1C", Currency_calc);

		CMatchingResultsTable results_table(ordersdb_table, table_1c);
		CCsvOutputWrapper(results_table).writeHeader(result);

		for (auto p = blocks1.begin(); p != blocks1.end(); ++p) {
			const std::string &adv1 = p->first;

			std::cout << adv1;
			auto p2 = blocks2.lower_bound(adv1);
			if (p2 != blocks2.end() && !(blocks2.key_comp()(adv1, p2->first))) {
				results_table = matching.match(p->second.position, \
												p->second.position + p->second.size,
												p2->second.position, \
												p2->second.position + p2->second.size, \
												false);
				blocks2.erase(p2);
			}
			else
				results_table = CMatchingResultsTable(CMatchingResultsTable::LEFT_PART, ordersdb_table, \
														p->second.position, \
														p->second.position + p->second.size, \
														false);

			CCsvOutputWrapper output_wrapper(results_table);
			result << output_wrapper;
			std::cout << " OK" << std::endl;
		}

		std::string empty_cells_filler;
		if (!blocks2.empty()) {
			size_t fields_count = ordersdb_table->getColumnsCount();
			for (size_t i = 0; i < fields_count; ++i)
				empty_cells_filler += ',';
		}
		for (auto &p2 : blocks2) {
			const std::string &adv2 = p2.first;

			std::cout << adv2;

			results_table = CMatchingResultsTable(CMatchingResultsTable::RIGHT_PART, table_1c, \
													p2.second.position, \
													p2.second.position + p2.second.size, \
													false);
			CCsvOutputWrapper output_wrapper(results_table, empty_cells_filler);
			result << output_wrapper;
			std::cout << " OK" << std::endl;
		}

		CCsvOutputWrapper(results_table).writeTail(result);

		result.close();
	}
	catch (XException &e) {

		std::wcerr << e.what();
		std::wcin.get();
		exit(1);
	}

	return 0;
}

std::shared_ptr<IDbConnection> createSQLiteConnection(const CPropertiesFile &props) {

	std::shared_ptr<IDbConnection> conn = std::make_shared<SQLiteConnection>();

	Tstring buffer;
	const Tchar *curr_prop = nullptr;
	std::string db_file_location;

	curr_prop = props.getStringProperty(_T("sqlite_db_file_location"), buffer);
	const char *p = UCS16_ToUTF8(curr_prop, -1, db_file_location);

	conn->Connect(p, 0, nullptr, nullptr, "acts");
	return conn;
}

CAdvocats fillAdvocatBlocks(std::shared_ptr<const ITable> in, \
	const size_t col_index) {
	CAdvocats advocats_blocks;

	size_t rec_count = in->getRecordsCount();
	std::string adv_name_prev = "";
	for (size_t i = 0; i < rec_count; ++i) {
		auto rec = in->getRecord(i);

		std::string adv_name = rec->getColValueAsCharArray(col_index);
		auto p = advocats_blocks.lower_bound(adv_name);
		if (p == advocats_blocks.end() || (advocats_blocks.key_comp()(adv_name, p->first))) {
			CAdvocatBlock block;
			typedef std::map<std::string, CAdvocatBlock>::value_type MVT;

			block.position = i;
			block.size = 1;
			advocats_blocks.insert(p, MVT(adv_name, block));
		}
		else {
			if (adv_name_prev != adv_name) {
				XException e(0, "the file was not sorted. Adv: ");
				e << adv_name;
				throw e;
			}
			++p->second.size;
		}
		adv_name_prev = adv_name;
	}

	return advocats_blocks;
}