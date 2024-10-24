#ifndef _DB_CONNECTION_H
#define _DB_CONNECTION_H

#include <queue>
#include <mutex>
#include <stdexcept>
#include <sql.h>
#include <sqlext.h>

#include "utils/Singleton.hpp"

namespace utils
{
    class DBConnection: public Singleton<DBConnection>
    {
    private:
        SQLHENV env;
        std::queue<SQLHDBC> connectionQ;
        std::mutex mLock;
        int qSize;

        SQLHDBC createConnection();
        DBConnection();
        friend class Singleton<DBConnection>;

    public:

        ~DBConnection();
        SQLHDBC getConnection();
        void releaseConnection(SQLHDBC connection);

        int checkError(SQLRETURN retcode, SQLHANDLE handle, SQLSMALLINT handle_type);

        int prepareStatement(std::string query, SQLHDBC connection, SQLHSTMT* stmt);
        void checkSchema(std::string schema, std::string create_query);
    };
}
#endif