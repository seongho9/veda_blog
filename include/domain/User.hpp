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
        std::string& get_id() {  return id;  }
        std::string& get_password() {   return password;    }
        std::string& get_nickname() {   return nickname;    }
        bool& get_is_valid() {  return is_valid;    }
        bool& get_is_manager() {    return is_manager;  }
    };
}
#endif