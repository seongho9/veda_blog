#ifndef _USER_LOGIC_H
#define _USER_LOGIC_H
#include <string>
#include <domain/User.hpp>

namespace logic
{
    class UserLogic
    {

    public:
        /// @brief 로그인 로직
        /// @param id 아이디
        /// @param password 비밀번호
        /// @param token 반환 토큰
        /// @return 0:성공 others:실패
        virtual int login(std::string id, std::string password, std::string* token) = 0;

        /// @brief 로그아웃
        /// @param id 아이디
        /// @param token 토큰
        /// @return 0:성공 others:실패
        virtual int logout(std::string id, std::string token) = 0;

        /// @brief 회원가임
        /// @param user 유저 객체
        /// @return 0:성공 others:실패
        virtual int register_user(domain::User user) = 0;

        /// @brief 회원탈퇴
        /// @param user 유저 객체
        /// @return 0:성공 others:실패
        virtual int delete_user(domain::User user) = 0;
    };
}
#endif