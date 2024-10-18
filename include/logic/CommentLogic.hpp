#ifndef _COMMENT_LOGIC_H
#define _COMMENT_LOGIC_H

#include "domain/Comment.hpp"
#include "domain/Post.hpp"
#include "domain/User.hpp"

#include <string>
#include <vector>

namespace logic
{
    class CommentLogic
    {
    public:
        /// @brief 댓글 추가
        /// @param comment 댓글 객체
        /// @param token 유저 식별을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int add_comment(domain::Comment comment, std::string token) = 0;
        /// @brief 댓글 수정
        /// @param comment 댓글 객체
        /// @param token 유저 식별을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int modify_comment(domain::Comment comment, std::string token) = 0;
        /// @brief 댓글 삭제
        /// @param id 댓글 id
        /// @param token 유저 식별을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int delete_comment(uint64_t id, std::string token) = 0;
        /// @brief 포스트의 댓글을 가져옴
        /// @param comment 값을 받아오는 댓글 벡터 객체
        /// @param postid 해당 게시글의 id
        /// @param token 유저 식별을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int get_commentlist(std::vector<domain::Comment>* comment, uint32_t postid, std::string token);
    };
}
#endif