#ifndef _POST_LOGIC_H
#define _POST_LOGIC_H

#include "domain/Post.hpp"
#include "domain/Comment.hpp"
#include "domain/User.hpp"

#include <string>
#include <vector>
#include <cstdint>

namespace logic
{
    class PostLogic
    {
    public:
        /// @brief 게시글 추가
        /// @param post 게시글 내용
        /// @param files 게시글에 첨부된 파일 내용
        /// @param token 회원 여부 확인을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int add_post(domain::Post post, const std::vector<std::string> files,  std::string token) = 0;

        /// @brief 게시글 삭제
        /// @param id 게시글 id
        /// @param token 회원 여부 확인을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int delete_post(uint32_t id, std::string token) = 0;

        /// @brief 게시글 추가
        /// @param post 게시글 내용
        /// @param files 게시글에 첨부된 파일 내용
        /// @param token 회원 여부 확인을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int modify_post(domain::Post post, const std::vector<std::string> files,  std::string token) = 0;

        /// @brief 게시글 목록을 가져옴
        /// @param post 받아올 벡터 객체
        /// @return 0:성공 others:실패
        virtual int get_postlist(std::vector<domain::Post>* post) = 0;

        /// @brief 게시글 내용을 가져옴
        /// @param post 받아올 게시글 객체
        /// @param comment 받아올 댓글 벡터 객체
        /// @param token 회원 여부 확인을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int get_post(domain::Post* post, std::vector<domain::Comment>* comment, std::string token)=0;

        /// @brief 게시글 id와 파일 이름을 이용하여 파일 내용을 가져옴
        /// @param file 파일 내용
        /// @param post_id 게시글 id
        /// @param file_name 파일 이름
        /// @param token 회원 여부 확인을 위한 토큰
        /// @return 0:성공 others:실패
        virtual int get_file(std::string* file, uint32_t post_id, std::string file_name,  std::string token)=0;
    };
}
#endif