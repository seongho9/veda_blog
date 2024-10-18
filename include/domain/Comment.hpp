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

        uint64_t& id();
        uint32_t& post_id();
        std::string& author();
        std::string& content();
        time_t& create_date();
        time_t& update_date();
        bool& is_valid();
    };
}
#endif