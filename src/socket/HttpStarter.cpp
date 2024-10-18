#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <iostream>

#include "socket/Http.hpp"
#include "spdlog/spdlog.h"


void HttpStarter::run(std::string addr, std::string port)
{
    try
    {
        auto const address = net::ip::make_address(addr);
        int port_int = std::stoi(port);
        unsigned short port_s = static_cast<unsigned short>(port_int);

        net::io_context ioc{1};

        std::unordered_map<std::string, http_handler_t> handler;

        register_handler(handler);

        std::shared_ptr<HttpListener> listener_ptr = 
            std::make_shared<HttpListener>(ioc, tcp::endpoint{address, port_s}, handler);

        listener_ptr.get()->do_accpet();
        ioc.run();
    }
    catch(std::exception& e)
    {
        spdlog::error("{}", e.what());
    }

}

void HttpStarter::register_handler(std::unordered_map<std::string, http_handler_t>& handler)
{
    //  여기에 컨트롤러를 생성
}
