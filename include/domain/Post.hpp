#ifndef _POST_H
#define _POST_H

#include <vector>
#include <string>
#include <cstdint>

namespace domain
{
    class Post
    {
        uint32_t id;
        std::string author;
        std::string title;
        std::string content;
        std::vector<std::string> file_name;
        time_t create_time;
        time_t update_time;
        bool  is_valid;

    public:
        uint32_t& id();
        std::string& author();
        std::string& title();
        std::string& content();
        std::vector<std::string>& file_name();
        std::string& create_time();
        std::string& update_time();

    };
}
#endif