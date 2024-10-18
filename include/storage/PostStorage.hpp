#ifndef _POST_STORAGE_H
#define _POST_STORAGE_H

#include "domain/User.hpp"
#include "domain/Post.hpp"
#include "domain/Comment.hpp"

namespace storage
{
    class PostStorage
    {
    public:

        /// @brief  게시글 DB에 추가
        /// @param post 추가하고자 하는 DB
        /// @return 0:성공 others:실패
        virtual int insert_post(domain::Post post)=0;

        /// @brief 게시글 리스트 가져오기
        /// @param post 가져오고자 하는 게시글 벡터
        /// @return 0:성공 others:실패
        virtual int get_postlist(std::vector<domain::Post>* post)=0;

        /// @brief 게시글 id를 이용 게시글 가져옴
        /// @param post 가져오고자하는 게시글
        /// @param comment 게시글의 댓글
        /// @param id 게시글 id
        /// @return 0:성공 others:실패
        virtual int get_post_byid(domain::Post* post, std::vector<domain::Comment>* comment, uint32_t id)=0;
        
        /// @brief 게시글 수정
        /// @param post 수정하고자 하는 게시글
        /// @param id 수정하고자 하는 게시글 id
        /// @return 0:성공 others:실패
        virtual int modify_post(domain::Post post, uint32_t id)=0;

        /// @brief 게시글 삭제
        /// @param id 삭제하고자 하는 게시글 id
        /// @return 0:성공 others:실패
        virtual int delete_post(uint32_t id)=0;

        /// @brief 저장한 파일경로 DB에 추가
        /// @param post_id 게시글 id
        /// @param filename 파일이름
        /// @param file_path 파일 경로
        /// @return 0:성공 others:실패
        virtual int insert_filepath(uint32_t post_id, std::string filename, std::string file_path)=0;

        /// @brief 파일이름과 게시글 id를 이용 파일경로를 가져옴
        /// @param path 가져올 파일 경로
        /// @param post_id 게시글 id
        /// @param filename 파일 이름
        /// @return 0:성공 others:실패
        virtual int get_filepath(std::string* path, uint32_t post_id, std::string filename)=0;
    };
    
} // namespace storage

#endif