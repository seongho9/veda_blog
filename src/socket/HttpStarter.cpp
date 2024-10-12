#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <iostream>

#include "socket/Http.hpp"
#include "spdlog/spdlog.h"
#include "storage/RoomStorage.hpp"
#include "storage/UserStorage.hpp"
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
    handler.insert({
        "/",
        [](const http::request<http::string_body>& req,http::response<http::string_body>& res) {
            spdlog::info("in /");
            res.set(http::field::server, "Boost.Beast");
            res.result(http::status::ok);
            res.version(11);
            res.set(http::field::content_type, "text/plain");
            res.body() = "ok";
            res.prepare_payload();
        }});
    handler.insert({
        "/user/login",
        [](const http::request<http::string_body>& req,http::response<http::string_body>& res) {
            
            UserStorage* userStorage = (UserStoragePostgres::getInstance());
            boost::property_tree::ptree pt;
            try{
                spdlog::info("/user/login");
                spdlog::info("{}", req.body());
                std::istringstream json_stream(req.body());

                boost::property_tree::read_json(json_stream, pt);

                std::string name = pt.get<std::string>("id");
                std::string password = pt.get<std::string>("password");
                
                spdlog::info("user \'{}\' request login", name);
                User user;
                if(!userStorage->findUserById(&user, name)){\
                    spdlog::info("{} {} {}", user.getId(), user.getName(), user.getPassword());
                    //  유저 비밀번호 매칭
                    if(user.getPassword() == password){
                        // login logic
                        res.set(http::field::server, "Boost.Beast");
                        res.result(http::status::ok);
                        res.version(11);
                        res.set(http::field::content_type, "text/plain");
                        res.body() = "success";
                        res.prepare_payload();
                    }
                    //  유저 비밀번호가 매칭되지 않음
                    else{
                        res.set(http::field::server, "Boost.Beast");
                        res.result(http::status::bad_request);
                        res.version(11);
                        res.set(http::field::content_type, "text/plain");
                        res.body() = "password not matched";
                        res.prepare_payload();                       
                    }
                }
                //  유저가 존재하지 않음
                else{
                        res.set(http::field::server, "Boost.Beast");
                        res.result(http::status::bad_request);
                        res.version(11);
                        res.set(http::field::content_type, "text/plain");
                        res.body() = "user is not exist";
                        res.prepare_payload(); 
                }
            }
            catch(const std::exception& ex)
            {
                spdlog::error("{}", ex.what());
                res.set(http::field::server, "Boost.Beast");
                res.result(http::status::bad_request);
                res.version(11);
                res.set(http::field::content_type, "text/plain");
                res.body() = "data is not valid";
                res.prepare_payload(); 
            }
        }});
    handler.insert({
        "/room/create",
        [](const http::request<http::string_body>& req,http::response<http::string_body>& res) {
            RoomStorage* roomStorage = RoomStoragePostgres::getInstance();
            
            spdlog::info("in /");
            res.set(http::field::server, "Boost.Beast");
            res.result(http::status::ok);
            res.version(11);
            res.set(http::field::content_type, "text/plain");
            res.body() = "ok";
            res.prepare_payload();
        }});
}
