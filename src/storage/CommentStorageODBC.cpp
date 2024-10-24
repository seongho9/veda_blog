#include "storage/CommentStorage.hpp"
#include "spdlog/spdlog.h"

using namespace storage;
using namespace std;

CommentStorageODBC::CommentStorageODBC()
{
    connectionPool = utils::DBConnection::getInstance();
}

int CommentStorageODBC::insert_comment(uint32_t post_id, domain::Comment comment)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();

    string query = 
        "INSERT INTO COMMENT (user_id, post_id, create_date, update_date, content, is_valid) VALUES(?,?,?,?,?,?)";
    if(connectionPool->prepareStatement(query, connection, &stmt)){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    //  user_id
    retcode = SQLBindParameter(stmt, 1,
        SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SQLULEN(comment.get_author().size()),0,
        (SQLCHAR*)comment.get_author().c_str(), comment.get_author().size(), NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  post_id
    retcode = SQLBindParameter(stmt, 2,
        SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, SQLULEN(sizeof(comment.get_post_id())),0,
        &comment.get_post_id(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  create_date
    retcode = SQLBindParameter(stmt, 3,
        SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, SQLULEN(sizeof(comment.get_create_date())), 0,
        &comment.get_create_date(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  update_date
    retcode = SQLBindParameter(stmt, 4,
        SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, SQLULEN(sizeof(comment.get_update_date())), 0,
        &comment.get_update_date(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  content
    retcode = SQLBindParameter(stmt, 5,
        SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SQLULEN(comment.get_content().length()), 0,
        (SQLCHAR*)comment.get_content().c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  is_valid
    bool valid = comment.get_is_valid() ? 1:0;
    retcode = SQLBindParameter(stmt, 6, 
        SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0,0,
        &valid, 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    
    retcode=SQLExecute(stmt);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        spdlog::error("comment insert query execution error");
        return 3;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);
    return 0;
}

int CommentStorageODBC::delete_comment(uint32_t comment_id)
{    
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();

    string query = 
        "UPDATE COMMENT SET is_valid=0 WHERE id=?";
    if(connectionPool->prepareStatement(query, connection, &stmt)){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }

    retcode = SQLBindParameter(stmt, 1, 
        SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0,
        &comment_id,0,NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }

    retcode = SQLExecute(stmt);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        spdlog::error("query execution error");
        return 3;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);
    return 0;
}

int CommentStorageODBC::modify_comment(domain::Comment comment)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();

    string query = 
        "UPDATE COMMENT SET content=?, update_date=? WHERE id=?";
    if(connectionPool->prepareStatement(query, connection, &stmt)){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    retcode = SQLBindParameter(stmt, 1,
        SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SQLULEN(comment.get_content().length()), 0,
        (SQLCHAR*)comment.get_content().c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }

    retcode = SQLBindParameter(stmt, 2, 
    SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0,
    &comment.get_update_date(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }

    retcode = SQLBindParameter(stmt, 3, 
        SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0,
        &comment.get_id(),0,NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }

    retcode = SQLExecute(stmt);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        spdlog::error("query execution error");
        return 3;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);
    return 0;   
}

int CommentStorageODBC::get_commentlist(vector<domain::Comment>* comment, uint32_t post_id)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();
    spdlog::debug("post id {}", post_id);

    string query = "SELECT id, post_id, user_id, content, create_date, update_date, is_valid FROM COMMENT WHERE post_id=?";
    if(connectionPool->prepareStatement(query, connection, &stmt)){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    //  post_id
    retcode = SQLBindParameter(stmt, 1, 
    SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0,
    (int*)&post_id, 0, NULL);
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

    retcode = SQLFetch(stmt);

    while(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER id, post_id;
        SQLCHAR author[20];
        SQLCHAR content[1000];
        SQLINTEGER create_date, update_date, is_valid;
        try{
            SQLGetData(stmt, 1, SQL_C_LONG, &id, sizeof(id), NULL);
            SQLGetData(stmt, 2, SQL_C_LONG, &post_id, sizeof(post_id), NULL);
            SQLGetData(stmt, 3, SQL_C_CHAR, &author, 20, NULL);
            SQLGetData(stmt, 4, SQL_C_CHAR, &content, 1000, NULL);
            SQLGetData(stmt, 5, SQL_C_LONG, &create_date, sizeof(create_date), NULL);
            SQLGetData(stmt, 6, SQL_C_LONG, &update_date, sizeof(update_date), NULL);
            SQLGetData(stmt, 7, SQL_C_LONG, &is_valid, sizeof(is_valid), NULL);
        }
        catch(std::exception& ex) {
            spdlog::error("Data Fetch Error");
            return 4; 
        }

        domain::Comment comm;
        comm.get_author().assign(reinterpret_cast<const char*>(author), 20);
        comm.get_content().assign(reinterpret_cast<const char*>(content), 1000);
        comm.get_id() = static_cast<uint32_t>(id);
        comm.get_post_id() = static_cast<uint32_t>(post_id);
        comm.get_create_date() = static_cast<time_t>(create_date);
        comm.get_update_date() = static_cast<time_t>(update_date);
        comm.get_is_valid() = is_valid == 0 ? false : true;
        spdlog::debug("comment {} {}", comm.get_author(), comm.get_id());
        comment->push_back(std::move(comm));

        retcode = SQLFetch(stmt);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);
    
    return 0;
}