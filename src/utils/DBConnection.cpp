#include "utils/DBConnection.hpp"
#include "spdlog/spdlog.h"

#include <string>
#include <sstream>

using namespace std;
using namespace utils;

DBConnection::DBConnection()
{
    this->qSize = 10;

    for(int i=0; i<qSize; i++) {
        SQLHDBC dbc = createConnection();
        connectionQ.push(dbc);
    }
}
DBConnection::~DBConnection()
{
    while(!connectionQ.empty()) {
        SQLHDBC connection = connectionQ.front();
        connectionQ.pop();

        SQLDisconnect(connection);
        SQLFreeHandle(SQL_HANDLE_DBC, connection);
    }
    SQLFreeHandle(SQL_HANDLE_ENV, this->env);
}

SQLHDBC DBConnection::createConnection()
{
    SQLHDBC connection;
    SQLRETURN retcode;

    //  ODBC 환경변수
    if(env == SQL_NULL_HENV) {
        retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &this->env);
        if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO){
            throw runtime_error("Failed to allocate environment handle");
        }
        SQLSetEnvAttr(this->env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    }
    //  연결 핸들 설정
    retcode = SQLAllocHandle(SQL_HANDLE_DBC, this->env, &connection);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        throw runtime_error("Failed to allocate connection handle");
    }

    //  DB connect
    retcode = SQLConnect(connection,
        //  server name 
        (SQLCHAR *)"SQLite", SQL_NTS,
        //  username
        NULL, 0,
        //  password
        NULL, 0);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_DBC, connection);
        // 오류 코드와 메시지를 추가하여 예외를 발생시킵니다.
        SQLCHAR sqlState[6], errorMessage[256];
        SQLINTEGER nativeError;
        SQLSMALLINT msgLength;
        
        SQLGetDiagRec(SQL_HANDLE_DBC, connection, 1, sqlState, &nativeError, errorMessage, sizeof(errorMessage), &msgLength);
        
        throw runtime_error("Failed to connect database server" + std::string((char*)errorMessage));
    }

    return connection;
}  

SQLHDBC DBConnection::getConnection()
{
    lock_guard<mutex> lock(this->mLock);
    if(this->connectionQ.empty()) {
        throw runtime_error("No available connections");
    }

    SQLHDBC conn = connectionQ.front();
    connectionQ.pop();

    return conn;
}

void DBConnection::releaseConnection(SQLHDBC conn)
{
    lock_guard<mutex> lock(this->mLock);
    connectionQ.push(conn);
}

int DBConnection::checkError(SQLRETURN retcode, SQLHANDLE handle, SQLSMALLINT handle_type)
{
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        // 성공이면 아무것도 하지 않음
        return 0; 
    }
    spdlog::error("DB Error");
    // 오류 정보 가져오기
    SQLCHAR state[1024];
    SQLCHAR message[1024];

    if (SQLGetDiagRec(handle_type, handle, 1, state, NULL, message, sizeof(message), NULL) == SQL_SUCCESS) {
        spdlog::error("ODBC Error {} : {}", 
            reinterpret_cast<const char*>(state), reinterpret_cast<const char*>(message));
    } 
    else {
        spdlog::error("Unknown ODBC Error");
    }
    return 1;
}

int DBConnection::prepareStatement(std::string query, SQLHDBC connection, SQLHSTMT* stmt)
{
    SQLRETURN retcode;
    //  핸들 할당
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, connection, stmt);
    if(retcode !=SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        spdlog::error("query statmente handle allocation error");
        return -1;
    }
    //  statement 쿼리 할당
    retcode = SQLPrepare(*stmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    if(retcode !=SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        spdlog::error("query statement allocation error");
        checkError(retcode, stmt, SQL_HANDLE_STMT);
        return -2;
    }

    return 0;
}
void DBConnection::checkSchema()
{
    SQLHDBC conn = getConnection();
    SQLHSTMT stmt;

    stringstream file_stream;
        file_stream << "CREATE TABLE FILE(";
        file_stream << " id INTEGER PRIMARY KEY AUTOINCREMENT,";
        file_stream << " path TEXT NOT NULL,";
        file_stream << " post_id INTEGER NOT NULL,";
        file_stream << " name TEXT NOT NULL,";
        file_stream << " FOREIGN KEY (post_id) REFERENCES POST(id))";
    prepareStatement(file_stream.str(), conn, &stmt);
    SQLExecute(stmt);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    stringstream comment_stream;
        comment_stream << "CREATE TABLE COMMENT(";
        comment_stream << " id INTEGER PRIMARY KEY AUTOINCREMENT,";
        comment_stream << " user_id TEXT NOT NULL,";
        comment_stream << " post_id INTEGER NOT NULL,";
        comment_stream << " content TEXT NOT NULL,";
        comment_stream << " create_date INTEGER NOT NULL,";
        comment_stream << " update_date INTEGER NOT NULL,";
        comment_stream << " is_valid INTEGER NOT NULL,";
        comment_stream << " FOREIGN KEY (user_id) REFERENCES USER(id)";
        comment_stream << " FOREIGN KEY (post_id) REFERENCES POST(id))";
    prepareStatement(comment_stream.str(), conn, &stmt);
    SQLExecute(stmt);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    stringstream post_stream;
        post_stream << "CREATE TABLE POST(";
        post_stream << " id INTEGER PRIMARY KEY AUTOINCREMENT,";
        post_stream << " user_id TEXT NOT NULL,";
        post_stream << " title TEXT NOT NULL,";
        post_stream << " content TEXT NOT NULL,";
        post_stream << " create_date INTEGER NOT NULL,";
        post_stream << " update_date INTEGER NOT NULL,";
        post_stream << " is_valid INTEGER NOT NULL,";
        post_stream << " FOREIGN KEY (user_id) REFERENCES USER(id))";
    prepareStatement(post_stream.str(), conn, &stmt);
    SQLExecute(stmt);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    
    stringstream user_stream;
        user_stream << "CREATE TABLE USER( ";
        user_stream << "id TEXT NOT NULL PRIMARY KEY, ";
        user_stream << "password TEXT NOT NULL, ";
        user_stream << "nickname TEXT NOT NULL, ";
        user_stream << "is_valid INTEGER NOT NULL, ";
        user_stream << "is_manager INTEGER NOT NULL)";
    prepareStatement(user_stream.str(), conn, &stmt);
    SQLExecute(stmt);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    releaseConnection(conn);

}