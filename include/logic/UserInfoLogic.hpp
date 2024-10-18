#ifndef _USERINFO_LOGIC
#define _USERINFO_LOGIC

#include "domain/Comment.hpp"
#include "domain/Post.hpp"
#include "domain/User.hpp"

#include <string>
#include <vector>

namespace logic
{
    class UserInfoLogic
    {
    public:

        /// @brief 해당 유저의 작성글 리스트 가져오기
        /// @param post 작성글을 가져오기 위한 반환 벡터
        /// @param token 유저 식별을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int get_postlist(std::vector<domain::Post>* post, std::string token) = 0;

        /// @brief 해당 유저가 작성한 댓글 가져오기
        /// @param comment 작성 댓글을 가져오기 위한 반환 벡터
        /// @param token 유저 식별을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int get_commentlist(std::vector<domain::Comment>* comment, std::string token) = 0;

        /// @brief 유저의 정보를 가져오기
        /// @param user 유저 정보를 가져오기 위한 객체
        /// @param token 유저 식별을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int get_userinfo(domain::User* user, std::string token) = 0;
    };
}

#endif