#ifndef _USER_H
#define _USER_H
#include <string>

namespace domain
{
    class User
    {
        std::string id;
        std::string password;
        std::string nickname;
        bool is_valid;
        bool is_manager;

    public:
        std::string& id();
        std::string& password();
        std::string& nickname();
        bool& is_valid();
        bool& is_manager();
    };
}
#endif