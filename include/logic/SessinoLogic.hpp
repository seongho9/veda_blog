#ifndef _SESSION_LOGIC_H
#define _SESSION_LOGIC_H

#include <string>
#include <unordered_map>

#include "utils/Singleton.hpp"

namespace logic
{
    class SessionLogic
    {
    public:

        /// @brief 로그인시 토큰 생성
        /// @param token 받아올 토큰 
        /// @param user_id 유저이름
        /// @return 0:성공 others:실패
        virtual int generate_token(std::string* token, std::string user_id) = 0;

        /// @brief 토큰을 통해 유저이름 반환
        /// @param user_id 받아올 유저이름
        /// @param token 토큰
        /// @return 0:성공 others:실패
        virtual int verify_token(std::string* user_id, std::string token) = 0;

        /// @brief 로그아웃시 토큰 삭제
        /// @param token 토큰
        /// @return 0:성공 others:실패
        virtual int delete_token(std::string token) = 0;
    };

    class SessionLogicMemory : public SessionLogic, public utils::Singleton<SessionLogicMemory>
    {
    private:
        std::unordered_map<std::string, std::string> _session;
        std::unordered_map<std::string, std::string> _session_verify;

        friend class utils::Singleton<SessionLogicMemory>;
        SessionLogicMemory();
    public:
        int generate_token(std::string* token, std::string user_id) override;
        int verify_token(std::string* user_id, std::string token) override;
        int delete_token(std::string token) override;
    };
    
} // namespace logic


#endif