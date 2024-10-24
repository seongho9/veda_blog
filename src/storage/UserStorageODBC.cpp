#include "storage/UserStorage.hpp"
#include "spdlog/spdlog.h"

using namespace storage;
using namespace std;

UserStorageODBC::UserStorageODBC()
{
    connectionPool = utils::DBConnection::getInstance();

    
}
int UserStorageODBC::insert_user(domain::User user)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();

    string query = "INSERT INTO USER (id, password, nickname, is_valid, is_manager) VALUES (?, ?, ?, ?, ?)";
    if(connectionPool->prepareStatement(query, connection, &stmt) != 0){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    //  id
    retcode = SQLBindParameter(stmt, 1,
        SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SQLULEN(user.get_id().size()), 0,
        (SQLCHAR*)user.get_id().c_str(), user.get_id().size(), NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  password
    retcode = SQLBindParameter(stmt, 2,
        SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SQLULEN(user.get_password().size()), 0,
        (SQLCHAR*)user.get_password().c_str(), user.get_password().size(), NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  nickname
    retcode = SQLBindParameter(stmt, 3,
        SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SQLULEN(user.get_nickname().size()), 0,
        (SQLCHAR*)user.get_nickname().c_str(), user.get_nickname().size(), NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  is_valid
    retcode = SQLBindParameter(stmt, 4,
        SQL_PARAM_INPUT, SQL_C_BIT, SQL_INTEGER, 0, 0,
        &user.get_is_valid(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  is_manager
    retcode = SQLBindParameter(stmt, 5,
        SQL_PARAM_INPUT, SQL_C_BIT, SQL_INTEGER, 0, 0,
        &user.get_is_manager(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    
    retcode = SQLExecute(stmt);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        spdlog::error("query execution error");
        return 3;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);
    return 0;
}

int UserStorageODBC::find_user_byid(domain::User* user, string userid)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();
    string query = "SELECT * FROM USER WHERE id=?";

    if(connectionPool->prepareStatement(query, connection, &stmt)){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    //  id
    retcode = SQLBindParameter(stmt, 1,
        SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, SQLULEN(userid.length()), 0,
        (SQLCHAR*)userid.c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }

    retcode = SQLExecute(stmt);
    if(retcode !=SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        spdlog::error("query execution error");
        return 3;
    }
    //  데이터 객체에 할당
    retcode = SQLFetch(stmt);
    if(retcode == SQL_NO_DATA) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);

        return 5;
    }
    if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        char id[20];
        char password[256];
        char nickname[20];
        bool is_valid;
        bool is_manager;
        try{
            //  id
            SQLGetData(stmt, 1, SQL_C_CHAR, id, sizeof(id), NULL);
            //  password
            SQLGetData(stmt, 2, SQL_C_CHAR, password, sizeof(password), NULL);
            //  nickname
            SQLGetData(stmt, 3, SQL_C_CHAR, nickname, sizeof(nickname), NULL);
            //  is_valid
            SQLGetData(stmt, 4, SQL_C_BIT, &is_valid, 1, NULL);
            //  is_manager
            SQLGetData(stmt, 5, SQL_C_BIT, &is_manager, 1, NULL);
        }
        catch(std::exception& ex){
            spdlog::error("Data Fetch Error");
            return 4;
        }

        user->get_id() = id;
        user->get_password() = password;
        user->get_nickname() = nickname;
        user->get_is_valid() = is_valid;
        user->get_is_manager() = is_manager;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);
    return 0;
}

int UserStorageODBC::delete_user_byid(string userid)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();
    string query = "UPDATE USER SET is_valid=0 WHERE id=?";
    if(connectionPool->prepareStatement(query, connection, &stmt)){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    retcode = SQLBindParameter(stmt, 1,
        SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, SQLULEN(userid.length()), 0,
        (SQLCHAR*)userid.c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }

    retcode = SQLExecute(stmt);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)) {
        
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        spdlog::error("query execution error");
        return 3;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);

    return 0;
}

int UserStorageODBC::get_post_byuser(vector<domain::Post>* post, string user_id)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();
    string query = "SELECT id, author, title, create_time, update_time, is_valid FROM POST WHERE user_id=?";
    if(connectionPool->prepareStatement(query, connection, &stmt)){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return -1;
    }

    retcode = SQLBindParameter(stmt, 1,
        SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, SQLULEN(user_id.length()), 0,
        (SQLCHAR*)user_id.c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }

    retcode = SQLExecute(stmt);
    if(retcode !=SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        spdlog::error("query execution error");
        return 3;
    }

    retcode = SQLFetch(stmt);
    while(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER id;
        SQLCHAR  author[20], title[20];
        SQLINTEGER create_time, update_time;
        SQLINTEGER is_valid;

        try{
            SQLGetData(stmt, 1, SQL_C_LONG, &id, sizeof(id), NULL);
            SQLGetData(stmt, 2, SQL_C_CHAR, author, sizeof(author), NULL);
            SQLGetData(stmt, 3, SQL_C_CHAR, title, sizeof(title), NULL);
            SQLGetData(stmt, 4, SQL_C_LONG, &create_time, sizeof(create_time), NULL);
            SQLGetData(stmt, 5, SQL_C_LONG, &update_time, sizeof(update_time), NULL);
            SQLGetData(stmt, 6, SQL_C_LONG, &is_valid, sizeof(is_valid), NULL);
        }
        catch(std::exception& ex) {
            spdlog::error("Data Fetch Error");
            return 4; 
        }
        domain::Post tmp;
        tmp.get_id() = static_cast<uint32_t>(id);
        tmp.get_author() = reinterpret_cast<const char*>(author);
        tmp.get_title() = reinterpret_cast<const char*>(title);
        tmp.get_create_time() = static_cast<time_t>(create_time);
        tmp.get_update_time() = static_cast<time_t>(update_time);
        tmp.get_is_valid() = is_valid == 0 ? false : true;

        post->push_back(std::move(tmp));

        retcode = SQLFetch(stmt);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);

    return 0;
}

int UserStorageODBC::get_comment_byuser(vector<domain::Comment>* comment, string user_id)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();
    string query = "SELECT id, post_id, create_date, update_date, content FROM COMMENT WHERE user_id=?";
    if(connectionPool->prepareStatement(query, connection, &stmt)){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return -1;
    }

    retcode = SQLBindParameter(stmt, 1,
        SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, SQLULEN(user_id.length()), 0,
        (SQLCHAR*)user_id.c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }

    retcode = SQLExecute(stmt);
    if(retcode !=SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        spdlog::error("query execution error");
        return 3;
    }  

    retcode = SQLFetch(stmt);
    while(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER id, post_id;
        SQLCHAR  content[256];
        SQLINTEGER create_time, update_time;
        SQLINTEGER is_valid;
        try{
        SQLGetData(stmt, 1, SQL_C_LONG, &id, sizeof(id), NULL);
        SQLGetData(stmt, 2, SQL_C_LONG, &post_id, sizeof(post_id), NULL);
        SQLGetData(stmt, 3, SQL_C_LONG, &create_time, sizeof(create_time), NULL);
        SQLGetData(stmt, 4, SQL_C_LONG, &update_time, sizeof(update_time), NULL);
        SQLGetData(stmt, 5, SQL_C_CHAR, content, sizeof(content), NULL);
        }
        catch(std::exception& ex) {
            spdlog::error("Data Fetch Error");
            return 4; 
        }
        domain::Comment comm;

        comm.get_author() = reinterpret_cast<const char*>("");
        comm.get_id() = static_cast<uint32_t>(id);
        comm.get_post_id() = static_cast<uint32_t>(post_id);
        comm.get_create_date() = static_cast<time_t>(create_time);
        comm.get_update_date() = static_cast<time_t>(update_time);
        comm.get_content() = reinterpret_cast<const char*>(content);

        comment->push_back(std::move(comm));
        retcode = SQLFetch(stmt);
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);

    return 0;  
}