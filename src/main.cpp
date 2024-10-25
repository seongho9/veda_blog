#include "socket/Http.hpp"
#include "spdlog/spdlog.h"
#include "utils/DBConnection.hpp"

int main(int argc, char const *argv[])
{
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] [%s:%# - %!] %v");
    spdlog::flush_on(spdlog::level::info);
    spdlog::set_level(spdlog::level::debug);

    utils::DBConnection *connection = utils::DBConnection::getInstance();

    connection->checkSchema();

    HttpStarter* http = new HttpStarter();
    http->run("0.0.0.0", "8000");



    delete http;
    
    return 0;
}
