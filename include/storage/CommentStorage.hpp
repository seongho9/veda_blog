#ifndef _COMMENT_STORAGE_H
#define _COMMENT_STORAGE_H

#include "domain/Comment.hpp"
#include "domain/Post.hpp"
#include "domain/User.hpp"

#include "utils/DBConnection.hpp"
#include "utils/Singleton.hpp"

namespace storage
{
    class CommentStorage
    {
    public:

        /// @brief 댓글 추가
        /// @param post_id 댓글이 추가되는 게시글 id
        /// @param comment 댓글 내용
        /// @return 0:성공 1:ODBC error 2:Binding error 3: Query Execution Error
        virtual int insert_comment(uint32_t post_id, domain::Comment comment)=0;

        /// @brief 댓글 삭제
        /// @param comment_id 삭제하고자 하는 댓글의 id
        /// @return 0:성공 1:ODBC error 2:Binding error 3: Query Execution Error
        virtual int delete_comment(uint32_t comment_id)=0;

        /// @brief 댓글 수정
        /// @param comment 수정하고자 하는 댓글
        /// @return 0:성공 1:ODBC error 2:Binding error 3: Query Execution Error
        virtual int modify_comment(domain::Comment comment)=0;

        /// @brief 게시글의 댓글을 새로고침
        /// @param comment 받아오고자 하는 댓글 벡터 객체
        /// @param post_id 게시글 id
        /// @return 0:성공 1:ODBC error 2:Binding error 3: Query Execution Error 4: Fetch Error
        virtual int get_commentlist(std::vector<domain::Comment>* comment, uint32_t post_id)=0;
    };

    class CommentStorageODBC : public CommentStorage, public utils::Singleton<CommentStorageODBC>
    {
    private:
        utils::DBConnection* connectionPool;

        CommentStorageODBC();
        friend class utils::Singleton<CommentStorageODBC>;
    public:
        int insert_comment(uint32_t post_id, domain::Comment comment);
        int delete_comment(uint32_t comment_id);
        int modify_comment(domain::Comment comment);
        int get_commentlist(std::vector<domain::Comment>* comment, uint32_t post_id);

    };
    
} // namespace storage

#endif