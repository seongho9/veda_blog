#ifndef _USER_STORAGE_H
#define _UESR_STORAGE_H

#include "domain/User.hpp"
#include "domain/Post.hpp"
#include "domain/Comment.hpp"

namespace storage
{
    class UserStorage
    {
    public:

        /// @brief 유저를 DB에 추가
        /// @param user 유저 객체
        /// @return 0:성공 others:실패
        virtual int insert_user(domain::User user) = 0;

        /// @brief 유저를 DB에서 검색
        /// @param user 유저객체
        /// @param userid 찾고자하는 id
        /// @return 0:성공 others:실패
        virtual int find_user_byid(domain::User* user, std::string userid) = 0;

        /// @brief id를 이용 유저 삭제
        /// @param userid 삭제하고자 하는 유저 id
        /// @return 0:성공 others:실패
        virtual int delete_user_byid(std::string userid) = 0;

        /// @brief 유저가 작성한 포스트 가져옴
        /// @param post 가져오려는 게시글의 벡터 객체
        /// @param user_id 유저 id
        /// @return 0:성공 others:실패
        virtual int get_post_byuser(std::vector<domain::Post>* post, std::string user_id) = 0;

        /// @brief 유저가 작성한 댓글 가져옴
        /// @param comment 가져오려는 댓글의 벡터 객체
        /// @param user_id 유저 id
        /// @return 0:성공 others:실패
        virtual int get_comment_byuser(std::vector<domain::Comment>* comment, std::string user_id) = 0;
    };
    
} // namespace storage


#endif