#include "utils/DBConnection.hpp"
#include "storage/UserStorage.hpp"

#include "spdlog/spdlog.h"

#include "gtest/gtest.h"
#include <sql.h>

class User_DB_Test : public ::testing::Test
{
protected:
    void SetUp() override {
        // 초기화 코드 또는 준비 작업을 여기에 추가
        utils::DBConnection* connection = utils::DBConnection::getInstance();

        SQLHDBC conn = connection->getConnection();
        SQLHSTMT stmt;
        //SQLRETURN ret;

        SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt);
        SQLRETURN cde = SQLExecDirect(stmt, (SQLCHAR*)"PRAGMA foreign_keys = ON", SQL_NTS);
        if(cde != SQL_SUCCESS){
            spdlog::debug("tq");
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        // INIT COMMENT
        connection->prepareStatement("DROP TABLE COMMENT", conn, &stmt);
        SQLExecute(stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        std::stringstream comment_stream;
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
        connection->prepareStatement(comment_stream.str(), conn, &stmt);
        SQLExecute(stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        
        // DELETE FILE
        connection->prepareStatement("DROP TABLE FILE", conn, &stmt);
        SQLExecute(stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        std::stringstream file_stream;
            file_stream << "CREATE TABLE FILE(";
            file_stream << " id INTEGER PRIMARY KEY AUTOINCREMENT,";
            file_stream << " path TEXT NOT NULL,";
            file_stream << " post_id INTEGER NOT NULL,";
            file_stream << " name TEXT NOT NULL,";
            file_stream << " FOREIGN KEY (post_id) REFERENCES POST(id))";
        connection->prepareStatement(comment_stream.str(), conn, &stmt);
        SQLExecute(stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        // INIT POST
        connection->prepareStatement("DROP TABLE POST", conn, &stmt);
        SQLExecute(stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        std::stringstream post_stream;
            post_stream << "CREATE TABLE POST(";
            post_stream << " id INTEGER PRIMARY KEY AUTOINCREMENT,";
            post_stream << " user_id TEXT NOT NULL,";
            post_stream << " title TEXT NOT NULL,";
            post_stream << " content TEXT NOT NULL,";
            post_stream << " create_date INTEGER NOT NULL,";
            post_stream << " update_date INTEGER NOT NULL,";
            post_stream << " is_valid INTEGER NOT NULL,";
            post_stream << " FOREIGN KEY (user_id) REFERENCES USER(id))";
        connection->prepareStatement(post_stream.str(), conn, &stmt);
        SQLExecute(stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        //  INIT USER
        connection->prepareStatement("DROP TABLE USER", conn, &stmt);
        SQLExecute(stmt);
        
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        std::stringstream user_stream;
            user_stream << "CREATE TABLE USER( ";
            user_stream << "id TEXT NOT NULL PRIMARY KEY, ";
            user_stream << "password TEXT NOT NULL, ";
            user_stream << "nickname TEXT NOT NULL, ";
            user_stream << "is_valid INTEGER NOT NULL, ";
            user_stream << "is_manager INTEGER NOT NULL)";
        connection->prepareStatement(user_stream.str(), conn, &stmt);
        SQLExecute(stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        connection->releaseConnection(conn);
        
        spdlog::debug("Test init");
        
    }

    // 테스트 케이스 실행 후 호출되는 메서드
    void TearDown() override {
        // 정리 코드 또는 후처리 작업을 여기에 추가
        spdlog::debug("Test cleanup");
    }
};

//  유저 정상 INSERT 테스트
TEST_F(User_DB_Test, INSERT_VALID)
{
    storage::UserStorage* storage = storage::UserStorageODBC::getInstance();

    //  given
    domain::User user;
    user.get_is_manager() = false;
    user.get_is_valid() = true;
    user.get_nickname() = "test_nick";
    user.get_password() = "test";
    user.get_id() = "test";
    //  when
    int retcode = storage->insert_user(user);
    //  then
    EXPECT_EQ(retcode, 0);
}

//  유저 정상 DELETE 테스트
TEST_F(User_DB_Test, DELETE_VALID)
{
    storage::UserStorage* storage = storage::UserStorageODBC::getInstance();
    //  prerequireement
    domain::User user;
    user.get_is_manager() = false;
    user.get_is_valid() = true;
    user.get_nickname() = "test_nick";
    user.get_password() = "test";
    user.get_id() = "test";
    storage->insert_user(user);

    //  given
    std::string id = "test";
    //  when
    int retcode = storage->delete_user_byid(id);
    //  then
    EXPECT_EQ(retcode, 0);
}

//  유저 정상 FIND 테스트
TEST_F(User_DB_Test, FIND_VALID)
{
    storage::UserStorage* storage = storage::UserStorageODBC::getInstance();
    //  requireement
    domain::User user;
    user.get_is_manager() = false;
    user.get_is_valid() = true;
    user.get_nickname() = "test_nick";
    user.get_password() = "test";
    user.get_id() = "test";
    storage->insert_user(user);

    //  given
    std::string id = "test";
    //  when
    domain::User test;
    int retcode = storage->find_user_byid(&test, id);
    //  then
    EXPECT_EQ(user.get_id(), test.get_id());
    EXPECT_EQ(user.get_is_manager(), test.get_is_manager());
    EXPECT_EQ(user.get_is_valid(), test.get_is_valid());
    EXPECT_EQ(user.get_nickname(), test.get_nickname());
    EXPECT_EQ(user.get_password(), user.get_password());
    EXPECT_EQ(retcode, 0);
    //  after
    storage->delete_user_byid(id);
}

//  유저 삽입 자세히
TEST_F(User_DB_Test, INSERT_USER_DETAIL)
{
    storage::UserStorage* storage = storage::UserStorageODBC::getInstance();
    //  given
    domain::User user;
    user.get_is_manager() = false;
    user.get_is_valid() = true;
    user.get_nickname() = "test_nick";
    user.get_password() = "test";
    user.get_id() = "test";
    //  when
    int retcode = storage->insert_user(user);
    //  then
    EXPECT_EQ(retcode, 0);    
    domain::User verify;
    retcode = storage->find_user_byid(&verify, user.get_id());

    EXPECT_EQ(retcode, 0);
    EXPECT_EQ(verify.get_id()==user.get_id(), true);
    EXPECT_EQ(verify.get_is_manager()==user.get_is_manager(), true);
    EXPECT_EQ(verify.get_is_valid()==user.get_is_valid(), true);
    EXPECT_EQ(verify.get_nickname()==user.get_nickname(), true);
    EXPECT_EQ(verify.get_password()==user.get_password(), true);
}

TEST_F(User_DB_Test, INSERT_DUPLIATE_USER_ID)
{
    storage::UserStorage* storage = storage::UserStorageODBC::getInstance();
    //  given
    domain::User user;
    user.get_is_manager() = false;
    user.get_is_valid() = true;
    user.get_nickname() = "test_nick";
    user.get_password() = "test";
    user.get_id() = "test";
    //  when
    storage->insert_user(user);
    int retcode = storage->insert_user(user);
    //  then
    EXPECT_NE(retcode, 0);
}

TEST_F(User_DB_Test, FIND_USER_NOT_EXIST)
{
    storage::UserStorage* storage = storage::UserStorageODBC::getInstance();
    //  given
    std::string user_id = "test";
    domain::User user;

    //  when
    int ret_code = storage->find_user_byid(&user, user_id);

    // then
    EXPECT_NE(ret_code, 0);
}

TEST_F(User_DB_Test, DELETE_USER_NOT_EXIST)
{
    storage::UserStorage* storage = storage::UserStorageODBC::getInstance();
    //  given
    std::string user_id = "test";
    domain::User user;

    //  when
    int ret_code = storage->delete_user_byid(user_id);

    // then
    EXPECT_NE(ret_code, 0);    
}


int main(int argc, char const **argv)
{
    ::testing::InitGoogleTest();
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] [%s:%# - %!] %v");
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::info);

    
    return RUN_ALL_TESTS();
}
