#include "utils/DBConnection.hpp"
#include "storage/PostStorage.hpp"
#include "storage/UserStorage.hpp"
#include "storage/CommentStorage.hpp"

#include "spdlog/spdlog.h"

#include <string>

#include "gtest/gtest.h"

class Post_DB_Test : public ::testing::Test
{
protected:
    // 테스트 케이스 실행 전 호출되는 메서드
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

TEST_F(Post_DB_Test, INSERT_VALID)
{
    //  pre-requirements
    storage::UserStorage* user_storage = storage::UserStorageODBC::getInstance();
    domain::User user;
    user.get_id() = "test_id";
    user.get_password() = "test_pw";
    user.get_nickname() = "test_nickname";
    user.get_is_manager() = false;
    user.get_is_valid() = true;
    user_storage->insert_user(user);
    storage::PostStorage* post_storage = storage::PostStorageODBC::getInstance();
    // given
    time_t cur_time = time(NULL);
    domain::Post post;
    post.get_author() = "test_id";
    post.get_title() = "title";
    post.get_content() = "content";
    post.get_create_time() =  cur_time;
    post.get_update_time() = cur_time;
    post.get_is_valid() = true;
    // when
    int retcode = post_storage->insert_post(post);
    // then
    EXPECT_EQ(retcode, 0);
}

TEST_F(Post_DB_Test, GET_LIST)
{
    //  pre-requirements
    storage::UserStorage* user_storage = storage::UserStorageODBC::getInstance();
    domain::User user;
    user.get_id() = "test_id";
    user.get_password() = "test_pw";
    user.get_nickname() = "test_nickname";
    user.get_is_manager() = false;
    user.get_is_valid() = true;
    user_storage->insert_user(user);
    storage::PostStorage* post_storage = storage::PostStorageODBC::getInstance();

    time_t cur_time = time(NULL);
    domain::Post post;
    post.get_author() = "test_id";
    post.get_title() = "title";
    post.get_content() = "content";
    post.get_create_time() =  cur_time;
    post.get_update_time() = cur_time;
    post.get_is_valid() = true;
    post_storage->insert_post(post);

    //given
    std::vector<domain::Post> list;

    int retcode = post_storage->get_postlist(&list);
    spdlog::debug("{}", list.size());
    EXPECT_EQ(retcode, 0);
    EXPECT_EQ(list.size(), 1);
}

TEST_F(Post_DB_Test, GET_LIST_BY_USER)
{
    //  pre-requirements
    storage::UserStorage* user_storage = storage::UserStorageODBC::getInstance();
    domain::User user1;
    user1.get_id() = "test_id";
    user1.get_password() = "test_pw";
    user1.get_nickname() = "test_nickname";
    user1.get_is_manager() = false;
    user1.get_is_valid() = true;
    user_storage->insert_user(user1);

    storage::PostStorage* post_storage = storage::PostStorageODBC::getInstance();
    //  given
    time_t cur_time = time(NULL);
    domain::Post post;
    post.get_author() = "test_id";
    post.get_title() = "title";
    post.get_content() = "content";
    post.get_create_time() =  cur_time;
    post.get_update_time() = cur_time;
    post.get_is_valid() = true;
    post_storage->insert_post(post);
    //  when
    domain::Post ret_post;
    std::vector<domain::Comment> comment_list;
    int ret = post_storage->get_post_byid(&ret_post, &comment_list, 1);
    // then
    EXPECT_EQ(ret, 0);

    EXPECT_EQ(strcmp(ret_post.get_author().c_str(), "test_id"), 0);
    EXPECT_EQ(comment_list.size(), 0);
}

TEST_F(Post_DB_Test, MODIFY_POST)
{
    //  pre-requirements
    storage::UserStorage* user_storage = storage::UserStorageODBC::getInstance();
    storage::PostStorage* post_storage = storage::PostStorageODBC::getInstance();

    domain::User user1;
    user1.get_id() = "test_id";
    user1.get_password() = "test_pw";
    user1.get_nickname() = "test_nickname";
    user1.get_is_manager() = false;
    user1.get_is_valid() = true;
    user_storage->insert_user(user1);

    //given
    time_t cur_time = time(NULL);
    domain::Post post;
    post.get_author() = "test_id";
    post.get_title() = "title";
    post.get_content() = "content";
    post.get_create_time() =  cur_time;
    post.get_update_time() = cur_time;
    post.get_is_valid() = true;

    post_storage->insert_post(post);
    
    post.get_title() = "modified title";
    post.get_content() = "modified conetent";
    sleep(1);
    //when
    int retcode = post_storage->modify_post(post, 1);

    //then
    domain::Post verify;
    std::vector<domain::Comment> comm;
    post_storage->get_post_byid(&verify, &comm, 1);
    spdlog::debug("{} {} ", post.get_author(), verify.get_author());
    spdlog::debug("{} {}", post.get_update_time(), verify.get_update_time());
    EXPECT_EQ(retcode, 0);
    EXPECT_EQ(strcmp(verify.get_author().c_str(), post.get_author().c_str()), 0);
    EXPECT_EQ(verify.get_content() == post.get_content(), false);
    EXPECT_NE((int)verify.get_update_time()-(int)post.get_update_time(), 0);
}

TEST_F(Post_DB_Test, DELETE_POST)
{
    //  pre-requirements
    storage::UserStorage* user_storage = storage::UserStorageODBC::getInstance();
    domain::User user1;
    user1.get_id() = "test_id";
    user1.get_password() = "test_pw";
    user1.get_nickname() = "test_nickname";
    user1.get_is_manager() = false;
    user1.get_is_valid() = true;
    user_storage->insert_user(user1);

    storage::PostStorage* post_storage = storage::PostStorageODBC::getInstance();
    //  given
    time_t cur_time = time(NULL);
    domain::Post post;
    post.get_author() = "test_id";
    post.get_title() = "title";
    post.get_content() = "content";
    post.get_create_time() =  cur_time;
    post.get_update_time() = cur_time;
    post.get_is_valid() = true;
    post_storage->insert_post(post);
    //  when
    domain::Post ret_post;
    std::vector<domain::Comment> comment_list;
    int ret = post_storage->delete_post( 1);
    // then
    EXPECT_EQ(ret, 0);

    std::vector<domain::Post> post_list;
    post_storage->get_postlist(&post_list);
    EXPECT_EQ(post_list.size(), 0);
}
int main(int argc, char const **argv)
{
    ::testing::InitGoogleTest();
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] [%s:%# - %!] %v");
    spdlog::set_level(spdlog::level::debug); 
    spdlog::flush_on(spdlog::level::info);
    
    return RUN_ALL_TESTS();
}
