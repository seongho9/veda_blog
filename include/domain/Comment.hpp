#ifndef _COMMENT_H
#define _COMMENT_H

#include <string>
#include <cstdint>

namespace domain
{
    class Comment
    {
        uint64_t id;
        uint32_t post_id;
        std::string author;
        std::string content;
        time_t create_date;
        time_t update_date;
        bool is_valid;
    public:
        uint64_t& get_id() { return id; }
        uint32_t& get_post_id() { return post_id; }
        std::string& get_author() { return author; }
        std::string& get_content() { return content; }
        time_t& get_create_date() { return create_date; }
        time_t& get_update_date() { return update_date; }
        bool& get_is_valid() { return is_valid; }
    };
}
#endif