#ifndef _POST_H
#define _POST_H

#include <vector>
#include <string>
#include <cstdint>
#include <ctime>

namespace domain
{
    class Post
    {
        uint32_t id;
        std::string author;
        std::string title;
        std::string content;
        std::vector<std::string> file_name;
        std::time_t create_time;
        std::time_t update_time;
        bool  is_valid;

    public:
        uint32_t& get_id() { return id; }
        std::string& get_author() { return author; }
        std::string& get_title() { return title; }
        std::string& get_content() { return content; }
        std::vector<std::string>& get_file_name() { return file_name; }
        std::time_t& get_create_time() { return create_time; }
        std::time_t& get_update_time() { return update_time; }
        bool& get_is_valid() { return is_valid; }

    };
}
#endif