#include "storage/PostStorage.hpp"
#include "spdlog/spdlog.h"

using namespace storage;
using namespace std;

PostStorageODBC::PostStorageODBC()
{
    connectionPool = utils::DBConnection::getInstance();
    commentStorage = CommentStorageODBC::getInstance();
}


int PostStorageODBC::insert_post(domain::Post post)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();

    string query = "INSERT INTO POST (user_id, title, content, create_date, update_date, is_valid) VALUES(?,?,?,?,?,?)";
    time_t cur_time = time(NULL);

    if(connectionPool->prepareStatement(query, connection, &stmt) != 0){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    //  author
    retcode = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
        SQL_C_CHAR, SQL_VARCHAR, 0, 0,
    (SQLCHAR*)post.get_author().c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  title
    retcode = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
        SQL_C_CHAR, SQL_VARCHAR, 0,0,
        (SQLCHAR*)post.get_title().c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  content
    retcode = SQLBindParameter(stmt, 3, SQL_PARAM_INPUT,
    SQL_C_CHAR, SQL_VARCHAR, 0,0,
    (SQLCHAR*)post.get_content().c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  creat_date
    retcode = SQLBindParameter(stmt, 4, SQL_PARAM_INPUT,
    SQL_C_LONG, SQL_INTEGER, 0, 0,
    &cur_time, 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  update_date
    retcode = SQLBindParameter(stmt, 5, SQL_PARAM_INPUT,
    SQL_C_LONG, SQL_INTEGER, 0, 0,
    &cur_time, 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  is_valid
    int valid;
    retcode = SQLBindParameter(stmt, 6, SQL_PARAM_INPUT,
    SQL_C_BIT, SQL_INTEGER, 0, 0,
    &post.get_is_valid(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }

    retcode = SQLExecute(stmt);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        spdlog::error("post insert query execution error");
        return 3;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);
    return 0;
}

int PostStorageODBC::insert_filepath(uint32_t post_id, string filename, string filepath)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();

    string query = "INSERT INTO FILE (post_id, path, name) VALUES(?,?,?)";
    if(connectionPool->prepareStatement(query, connection, &stmt) != 0){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    //  post_id
    retcode=SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
    SQL_C_LONG, SQL_INTEGER, 0,0,
    &post_id, 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    // path
    retcode=SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
    SQL_C_CHAR, SQL_VARCHAR, 0,0,
    (SQLCHAR*)filepath.c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  name
    retcode=SQLBindParameter(stmt, 3, SQL_PARAM_INPUT,
    SQL_C_CHAR, SQL_VARCHAR, 0,0,
    (SQLCHAR*)filename.c_str(), 0, NULL);
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

int PostStorageODBC::get_postlist(vector<domain::Post>* post)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();

    string query = "SELECT id, user_id, title, content, create_date, update_date FROM POST WHERE NOT is_valid=0";
    if(connectionPool->prepareStatement(query, connection, &stmt) != 0){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }   

    retcode = SQLExecute(stmt);
    if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 3;
    }

    retcode = SQLFetch(stmt);
    while(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER id;
        SQLCHAR author[20], title[100], content[10240];
        SQLINTEGER create_date, update_date;

        try{
            SQLGetData(stmt, 1, SQL_C_LONG, &id, sizeof(id), NULL);
            SQLGetData(stmt, 2, SQL_C_CHAR, author, 20, NULL);
            SQLGetData(stmt, 3, SQL_C_CHAR, title, 100, NULL);
            SQLGetData(stmt, 4, SQL_C_CHAR, content, 10240, NULL);
            SQLGetData(stmt, 5, SQL_C_LONG, &create_date, sizeof(SQLINTEGER), NULL);
            SQLGetData(stmt, 6, SQL_C_LONG, &update_date, sizeof(SQLINTEGER), NULL);
        }
        catch(std::exception& ex) {
            spdlog::error("Data Fetch Error");
            return 4; 
        }
        domain::Post tmp;

        tmp.get_id() = static_cast<uint32_t>(id);

        tmp.get_author().assign(reinterpret_cast<const char*>(author), 20);
        tmp.get_title().assign(reinterpret_cast<const char*>(title), 100);
        tmp.get_content().assign(reinterpret_cast<const char*>(content), 10240);

        tmp.get_create_time() = static_cast<time_t>(create_date);
        tmp.get_update_time() = static_cast<time_t>(update_date);

        spdlog::debug("id {}", tmp.get_id());
        post->push_back(tmp);
        spdlog::debug("after push back");
        spdlog::debug("size {}", post->size());
        retcode = SQLFetch(stmt);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);
    return 0;
}

int PostStorageODBC::get_post_byid(domain::Post* post, vector<domain::Comment>* comment, uint32_t id)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();
    // post fetch
    string query = "SELECT id, user_id, title, content, create_date, update_date FROM POST WHERE id=?";
    if(connectionPool->prepareStatement(query, connection, &stmt) != 0){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    retcode = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
    SQL_C_LONG, SQL_INTEGER, 0, 0,
    &id, 0, NULL);
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

    retcode = SQLFetch(stmt);
    if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER id;
        SQLCHAR author[20], title[100], content[10240];
        SQLINTEGER create_date, update_date;
        try{
            SQLGetData(stmt, 1, SQL_C_LONG, &id, sizeof(id), NULL);
            SQLGetData(stmt, 2, SQL_C_CHAR, author, 20, NULL);
            SQLGetData(stmt, 3, SQL_C_CHAR, title, 100, NULL);
            SQLGetData(stmt, 4, SQL_C_CHAR, content, 10240, NULL);
            SQLGetData(stmt, 5, SQL_C_LONG, &create_date, sizeof(SQLINTEGER), NULL);
            SQLGetData(stmt, 6, SQL_C_LONG, &update_date, sizeof(SQLINTEGER), NULL);
        }
        catch(std::exception& ex) {
            spdlog::error("Data Fetch Error");
            return 4; 
        }

        post->get_author().assign(reinterpret_cast<const char*>(author), 20);
        post->get_title().assign(reinterpret_cast<const char*>(title), 100);
        post->get_content().assign(reinterpret_cast<const char*>(content), 10240);
        post->get_id() = id;
        post->get_create_time() = create_date;
        post->get_update_time() = update_date;
    }
    else{
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 5;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);

    //  file fetch
    string file_query = "SELECT name FROM FILE WHERE post_id=?";
    if(connectionPool->prepareStatement(query, connection, &stmt) != 0){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    retcode = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
    SQL_C_LONG, SQL_INTEGER, 0, 0,
    &id, 0, NULL);
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

    retcode = SQLFetch(stmt);
    while(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        try{
            SQLCHAR name[20];
            SQLGetData(stmt, 1, SQL_C_CHAR, name, 20, NULL);
            post->get_file_name().push_back(string(reinterpret_cast<const char*>(name),20));
        }
        catch(std::exception& ex){
            spdlog::error("Data Fetch Error");
            return 4; 
        }
        retcode = SQLFetch(stmt);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);

    //  comment fetch
    int ret_comment = commentStorage->get_commentlist(comment, id);
    if(ret_comment){
        spdlog::error("Cannot fetch comment");
        return ret_comment;
    }

    return 0;
}

int PostStorageODBC::delete_post(uint32_t id)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();

    string query = "UPDATE POST SET is_valid=0 WHERE id=?";
    if(connectionPool->prepareStatement(query, connection, &stmt) != 0){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }

    retcode = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
    SQL_C_LONG, SQL_INTEGER, 0,0,
    &id, 0, NULL);
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

int PostStorageODBC::modify_post(domain::Post post, uint32_t id)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();

    time_t update_time = time(NULL);

    string query = "UPDATE POST SET content=?, update_date=?, title=? WHERE id=?";
    if(connectionPool->prepareStatement(query, connection, &stmt) != 0){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    //  content
    retcode = SQLBindParameter(stmt,1,SQL_PARAM_INPUT,
    SQL_C_CHAR, SQL_VARCHAR, 0,0,
    (SQLCHAR*)post.get_content().c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  update_date
    retcode = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
    SQL_C_LONG, SQL_INTEGER, 0,0,
    &update_time, 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  title
    retcode = SQLBindParameter(stmt,3,SQL_PARAM_INPUT,
    SQL_C_CHAR, SQL_VARCHAR, 0,0,
    (SQLCHAR*)post.get_title().c_str(), 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  id condition
    retcode = SQLBindParameter(stmt, 4, SQL_PARAM_INPUT,
    SQL_C_LONG, SQL_INTEGER, 0,0,
    &id, 0, NULL);
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

int PostStorageODBC::get_filepath(string* path, uint32_t post_id, string filename)
{
    SQLHSTMT stmt;
    SQLRETURN retcode;
    SQLHDBC connection = connectionPool->getConnection();

    string query = "SELECT path FROM FILE WHERE post_id=? AND name=?";
    if(connectionPool->prepareStatement(query, connection, &stmt) != 0){
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        connectionPool->releaseConnection(connection);
        return 1;
    }
    //  post_id
    retcode = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
    SQL_C_LONG, SQL_INTEGER, 0,0,
    &post_id, 0, NULL);
    if(connectionPool->checkError(retcode, stmt, SQL_HANDLE_STMT)){
        return 2;
    }
    //  file_name
    retcode = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
    SQL_C_CHAR, SQL_VARCHAR, 0,0,
    (SQLCHAR*)filename.c_str(), 0, NULL);
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

    SQLCHAR file_path[100];
    retcode = SQLFetch(stmt);
    if(retcode !=SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO){
        return 4;
    }
    retcode = SQLGetData(stmt, 1, SQL_C_CHAR, file_path, 100, NULL);
    if(retcode !=SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO){
        return 4;
    }
    
    path->assign(reinterpret_cast<const char*>(file_path), 100);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    connectionPool->releaseConnection(connection);

    return 0;
}