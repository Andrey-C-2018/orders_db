#pragma once
#include <db/IDbStatement.h>

struct IDbConnection;

class PaginalStatement : public IDbStatement {
	std::shared_ptr<IDbStatement> stmt, stmt_rec_count;
	int field_id_index;

public:
	PaginalStatement(std::shared_ptr<IDbConnection> conn_, \
						const char *query, int field_id_index_);

	PaginalStatement(const PaginalStatement &obj) = delete;
	PaginalStatement(PaginalStatement &&obj) = default;
	PaginalStatement& operator=(const PaginalStatement &obj) = delete;
	PaginalStatement& operator=(PaginalStatement &&obj) = default;

	void init(std::shared_ptr<IDbConnection> conn_, const char* query);

	size_t getParamsCount() const override;
	void bindValue(const size_t param_no, const int value) override;

	void bindValue(const size_t param_no, const char *value) override;
	void bindValue(const size_t param_no, const wchar_t *value) override;
	void bindValue(const size_t param_no, const ImmutableString<char> value) override;
	void bindValue(const size_t param_no, const ImmutableString<wchar_t> value) override;

	void bindValue(const size_t param_no, const CDate &value) override;

	void bindNull(const size_t param_no) override;

	std::shared_ptr<IDbResultSet> exec() override;
	record_t execScalar() override;
	std::shared_ptr<IDbResultSetMetadata> getResultSetMetadata() override;

	virtual ~PaginalStatement();
};