#include "utils/DBConnection.hpp"

#include "storage/UserStorage.hpp"
#include "storage/PostStorage.hpp"
#include "storage/CommentStorage.hpp"

#include "spdlog/spdlog.h"

#include "gtest/gtest.h"

#include <string>
#include <sql.h>

class Comment_DB_Test : public ::testing::Test
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

        connection->releaseConnection(conn);

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


        spdlog::debug("Test init");
        
    }

    // 테스트 케이스 실행 후 호출되는 메서드
    void TearDown() override {
        // 정리 코드 또는 후처리 작업을 여기에 추가
        spdlog::debug("Test cleanup");
    }
};

TEST_F(Comment_DB_Test, INSERT_VALID)
{
    //  pre-requirements
    storage::UserStorage* user_storage = storage::UserStorageODBC::getInstance();
    domain::User user;
    user.get_id() = "test_id";
    user.get_password() = "test_pw";
    user.get_nickname() = "test_nickname12";
    user.get_is_manager() = false;
    user.get_is_valid() = true;
    user_storage->insert_user(user);
    //  post id 1
    storage::PostStorage* post_storage = storage::PostStorageODBC::getInstance();
    domain::Post post;
    post.get_author() = "test_id";
    post.get_content() = "post_conten12t";
    post.get_create_time() = time(NULL);
    post.get_update_time() = time(NULL);
    post.get_is_valid() = false;
    post.get_title() = "post_title";
    post_storage->insert_post(post);

    storage::CommentStorage* comment_stroage = storage::CommentStorageODBC::getInstance();
    //given
    domain::Comment comment;
    comment.get_author() = "test_id";
    comment.get_content() = "comment";
    comment.get_create_date() = time(NULL);
    comment.get_update_date() = time(NULL);
    comment.get_is_valid() = time(NULL);
    comment.get_post_id() = 1u;

    comment_stroage->insert_comment(1, comment);
    //when
    int retcode = comment_stroage->insert_comment(1u, comment);
    //then
    EXPECT_EQ(retcode, 0);

}

TEST_F(Comment_DB_Test, GET_VALID)
{
    //  pre-requirements
    storage::UserStorage* user_storage = storage::UserStorageODBC::getInstance();
    domain::User user;
    user.get_id() = "test_id";
    user.get_password() = "test_pw";
    user.get_nickname() = "test_nickname";
    user.get_is_manager() = false;
    user.get_is_valid() = true;
    if(!user_storage->insert_user(user)){
        spdlog::debug("user inserted");
    }
    //  post id 1
    storage::PostStorage* post_storage = storage::PostStorageODBC::getInstance();
    domain::Post post;
    post.get_author() = "test_id";
    post.get_content() = "post_content";
    post.get_create_time() = time(NULL);
    post.get_update_time() = time(NULL);
    post.get_is_valid() = true;
    post.get_title() = "post_title";
    if(!post_storage->insert_post(post)) {
        spdlog::debug("post inserted");
    }
    storage::CommentStorage* comment_stroage = storage::CommentStorageODBC::getInstance();
    //given
    domain::Comment comment;
    comment.get_author() = "test_id";
    comment.get_content() = "comment";
    comment.get_create_date() = time(NULL);
    comment.get_update_date() = time(NULL);
    comment.get_is_valid() = true;
    comment.get_post_id() = 1u;
    comment_stroage->insert_comment(1, comment);
    //when
    std::vector<domain::Comment> list;
    int ret = comment_stroage->get_commentlist(&list, 1u);

    //then
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(list.size(), 1);
}

TEST_F(Comment_DB_Test, DELETE_VALID)
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
    //  post id 1
    storage::PostStorage* post_storage = storage::PostStorageODBC::getInstance();
    domain::Post post;
    post.get_author() = "test_id";
    post.get_content() = "post_content";
    post.get_create_time() = time(NULL);
    post.get_update_time() = time(NULL);
    post.get_is_valid() = true;
    post.get_title() = "post_title";
    post_storage->insert_post(post);

    storage::CommentStorage* comment_stroage = storage::CommentStorageODBC::getInstance();
    domain::Comment comment;
    comment.get_author() = "test_id";
    comment.get_content() = "comment";
    comment.get_create_date() = time(NULL);
    comment.get_update_date() = time(NULL);
    comment.get_is_valid() = time(NULL);
    comment.get_post_id() = 1u;
    comment_stroage->insert_comment(1u, comment);

    //given
    std::vector<domain::Comment> list;
    comment_stroage->get_commentlist(&list, 1u);
    
    int retcode = comment_stroage->delete_comment(list[0].get_id());
    //then
    EXPECT_EQ(retcode, 0);
}

TEST_F(Comment_DB_Test, MODIFY_VALID)
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
    //  post id 1
    storage::PostStorage* post_storage = storage::PostStorageODBC::getInstance();
    domain::Post post;
    post.get_author() = "test_id";
    post.get_content() = "post_content";
    post.get_create_time() = time(NULL);
    post.get_update_time() = time(NULL);
    post.get_is_valid() = true;
    post.get_title() = "post_title";
    post_storage->insert_post(post);

    storage::CommentStorage* comment_stroage = storage::CommentStorageODBC::getInstance();
    domain::Comment comment;
    comment.get_author() = "test_id";
    comment.get_content() = "comment";
    comment.get_create_date() = time(NULL);
    comment.get_update_date() = time(NULL);
    comment.get_is_valid() = time(NULL);
    comment.get_post_id() = 1u;
    comment_stroage->insert_comment(1, comment);
    //given
    std::vector<domain::Comment> list;
    comment_stroage->get_commentlist(&list, 1u);
    domain::Comment comm = list.at(0);
    //when
    comm.get_content()="modify";
    int retcode = comment_stroage->modify_comment(comm);

    //then
    EXPECT_EQ(retcode, 0);
}

int main(int argc, char const **argv)
{
    ::testing::InitGoogleTest();
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] [%s:%# - %!] %v");
    spdlog::flush_on(spdlog::level::info);

    
    return RUN_ALL_TESTS();
}
