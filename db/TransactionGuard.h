#pragma once
#include <memory>
#include "dll_definitions.h"

struct IDbConnection;

class DBLIB_DLL_EXPORT TransactionGuard final {
    bool started, commited;
    std::shared_ptr<IDbConnection> conn;

public:
    explicit TransactionGuard(std::shared_ptr<IDbConnection> conn_);

    TransactionGuard(const TransactionGuard &obj) = delete;
    TransactionGuard(TransactionGuard &&obj) = default;
    TransactionGuard& operator=(const TransactionGuard &obj) = delete;
    TransactionGuard& operator=(TransactionGuard &&obj) = default;

    void commit();
    void rollback();

    ~TransactionGuard();
};
