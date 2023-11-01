#include "TransactionGuard.h"
#include "IDbConnection.h"

TransactionGuard::TransactionGuard(std::shared_ptr<IDbConnection> conn_) : \
                                    started(false), commited(false), conn(conn_) {

    conn->setAutocommitMode(true);
    started = true;
}

void TransactionGuard::commit() {

    if (started && !commited) {
        conn->commit();
        commited = true;
    }
}

void TransactionGuard::rollback() {

    if (started && !commited) {
        conn->rollback();
        commited = true;
    }
}

TransactionGuard::~TransactionGuard() {

    if (started) {
        if (!commited) conn->rollback();

        conn->setAutocommitMode(false);
        started = false;
    }
}
