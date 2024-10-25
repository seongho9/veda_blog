#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>  // std::put_time

#include "socket/Http.hpp"
#include "spdlog/spdlog.h"

#include "logic/UserLogic.hpp"
#include "logic/UserInfoLogic.hpp"
#include "logic/PostLogic.hpp"
#include "logic/CommentLogic.hpp"


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
    //  /user/login
    handler.insert({"/user/login",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::post){
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            logic::UserLogic* userLogic = logic::UserLogicImpl::getInstance();
            
            std::stringstream req_stream(req.body());
            boost::property_tree::read_json(req_stream, pt);
            std::string token;
            int ret = userLogic->login(pt.get<std::string>("id"), pt.get<std::string>("password"), &token);

            if(ret==1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "user is not exist");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret==2) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "password not matched");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret==3) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "already login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            pt.put("token", token);
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /user/logout
    handler.insert({"/user/logout",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::post){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            std::string token = req.at(http::field::authorization).to_string();
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }

            logic::UserLogic* userLogic = logic::UserLogicImpl::getInstance();
            
            int ret = userLogic->logout("", token);

            if(ret==1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            pt.put("status", "success");
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    //  /user/register
    handler.insert({"/user/register",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::post){
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            logic::UserLogic* userLogic = logic::UserLogicImpl::getInstance();
            
            std::stringstream req_stream(req.body());
            boost::property_tree::read_json(req_stream, pt);
            std::string token;
            domain::User user;
            user.get_id() = pt.get<std::string>("id");
            user.get_password() = pt.get<std::string>("password");
            user.get_nickname() = pt.get<std::string>("nickname");
            user.get_is_valid() = true;
            user.get_is_manager() = pt.get<std::string>("is_manager") == "0" ? false:true;
            int ret = userLogic->register_user(user);

            if(ret==1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "data is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret==2) {
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "interal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret==3) {
                pt.clear();
                res.result(http::status::conflict);
                pt.put("message", "duplicate user");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            pt.put("status", "success");
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    //  /user/delete
    handler.insert({"/user/delete",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::post){
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            logic::UserLogic* userLogic = logic::UserLogicImpl::getInstance();
            
            std::stringstream req_stream(req.body());
            boost::property_tree::read_json(req_stream, pt);
            std::string token;
            domain::User user;
            user.get_id() = pt.get<std::string>("id");
            user.get_password() = pt.get<std::string>("password");
            int ret = userLogic->delete_user(user);

            if(ret==1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "data is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret==2) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            pt.put("status","success");
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /userinfo/posts
    handler.insert({"/userinfo/posts",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::get){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            spdlog::info("before token");
            std::string token = req.at(http::field::authorization).to_string();
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }
            
            logic::UserInfoLogic* userinfo_logic = logic::UserInfoLogicImpl::getInstance();
            
            std::vector<domain::Post> posts;
            int ret = userinfo_logic->get_postlist(&posts, token);

            if(ret==-11) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret!=0){
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "internal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            for(auto& post: posts){
                boost::property_tree::ptree post_tree;
                post_tree.put("id", std::to_string(post.get_id()));
                post_tree.put("author", post.get_author());
                post_tree.put("title",post.get_title());
                std::tm* tm_ptr = std::gmtime(&post.get_update_time());
                std::stringstream ss;
                ss<<std::put_time(tm_ptr, "%Y-%m-%d %H:%M");
                post_tree.put("update_date", ss.str());

                pt.add_child("posts", post_tree);
            }
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /userinfo/comments
    handler.insert({"/userinfo/comments",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::get){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }

            std::string token = req.at(http::field::authorization).to_string();
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }
            
            logic::UserInfoLogic* userinfo_logic = logic::UserInfoLogicImpl::getInstance();
            
            std::vector<domain::Comment> commenets;
            int ret = userinfo_logic->get_commentlist(&commenets, token);

            if(ret==-11) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret!=0){
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "internal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            for(auto& commenet: commenets){
                boost::property_tree::ptree post_tree;
                post_tree.put("id", std::to_string(commenet.get_id()));
                post_tree.put("author", commenet.get_author());
                post_tree.put("title",commenet.get_content());
                std::tm* tm_ptr = std::gmtime(&commenet.get_update_date());
                std::stringstream ss;
                ss<<std::put_time(tm_ptr, "%Y-%m-%d %H:%M");
                post_tree.put("update_date", ss.str());

                pt.add_child("comments", post_tree);
            }
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /userinfo/info
    handler.insert({"/userinfo/info",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::get){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            std::string token = req.at(http::field::authorization).to_string();
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }

            logic::UserInfoLogic* userinfo_logic = logic::UserInfoLogicImpl::getInstance();
            domain::User user;

            int ret = userinfo_logic->get_userinfo(&user, token);

            if(ret) {
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "interal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            pt.put("id", user.get_id());
            pt.put("nickname", user.get_nickname());
            pt.put("is_manager", (user.get_is_manager() ? "true":"false"));
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });    
    // /post/register
    handler.insert({"/post/register",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::post){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            std::string token = req.at(http::field::authorization).to_string();
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }

            logic::PostLogic* post_logic = logic::PostLogicImpl::getInstance();
            
            std::stringstream req_stream(req.body());
            boost::property_tree::read_json(req_stream, pt);

            domain::Post post;
            post.get_content() = pt.get<std::string>("content");
            post.get_title() = pt.get<std::string>("title");
            post.get_is_valid() = true;
            std::vector<std::string> file_mock;

            int ret = post_logic->add_post(post, file_mock, token);
            if(ret==-1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret) {
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "interal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            pt.put("status", "success");
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /post/delete
    handler.insert({"/post/delete",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::post){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            std::string token = req.at(http::field::authorization).to_string();
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }

            logic::PostLogic* post_logic = logic::PostLogicImpl::getInstance();
            
            std::stringstream req_stream(req.body());
            boost::property_tree::read_json(req_stream, pt);

            domain::Post post;
            uint32_t id = pt.get<uint32_t>("id");

            int ret = post_logic->delete_post(id, token);
            if(ret==-1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret) {
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "interal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            pt.put("status", "success");
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /post/register
    handler.insert({"/post/modify",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::post){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            std::string token = req.at(http::field::authorization).to_string();
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }

            logic::PostLogic* post_logic = logic::PostLogicImpl::getInstance();
            
            std::stringstream req_stream(req.body());
            boost::property_tree::read_json(req_stream, pt);

            domain::Post post;
            post.get_id() = pt.get<uint32_t>("id");
            post.get_content() = pt.get<std::string>("content");
            post.get_title() = pt.get<std::string>("title");
            post.get_is_valid() = true;
            std::vector<std::string> file_mock;

            int ret = post_logic->modify_post(post, file_mock, token);
            
            if(ret==-1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret) {
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "interal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            pt.put("status", "success");
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /post/list
    handler.insert({"/post/list",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::get){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            logic::PostLogic* post_logic = logic::PostLogicImpl::getInstance();
            
            std::vector<domain::Post> post_list;
            int ret = post_logic->get_postlist(&post_list);
            
            if(ret==-1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret) {
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "interal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            boost::property_tree::ptree post_tree;
            for(auto& post: post_list){
                boost::property_tree::ptree post_element;

                post_element.put("id", std::to_string(post.get_id()));
                post_element.put("author", post.get_author().substr(0, post.get_author().find_first_of('\0')));
                post_element.put("title",post.get_title().substr(0, post.get_title().find_first_of('\0')));
                std::tm* tm_ptr = std::gmtime(&post.get_update_time());
                std::stringstream ss;
                ss<<std::put_time(tm_ptr, "%Y-%m-%d %H:%M");
                post_element.put("update_date", ss.str());

                post_tree.push_back({"", post_element});
            }
            pt.add_child("posts", post_tree);
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /post?id={id}
    handler.insert({"/post",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::get){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            std::string token = req.at(http::field::authorization).to_string();
            
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }

            logic::PostLogic* post_logic = logic::PostLogicImpl::getInstance();

            domain::Post post;
            std::vector<domain::Comment> comments;
            
            std::string query_param = req.target().to_string();
            size_t id_pos = query_param.find("id");
            if(id_pos==std::string::npos){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "id is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            size_t start_pos = id_pos+3;
            size_t end_pos = query_param.find('&', start_pos);

            uint32_t id = static_cast<uint32_t>(std::stoul(query_param.substr(start_pos, end_pos)));
            post.get_id() = id;
            int ret = post_logic->get_post(&post, &comments, token);
            
            
            if(ret==-1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret) {
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "interal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            pt.put("id", post.get_id());
            pt.put("author", post.get_author().substr(0, post.get_author().find_first_of('\0')));
            pt.put("title", post.get_title().substr(0, post.get_title().find_first_of('\0')));
            pt.put("content", post.get_content().substr(0, post.get_content().find_first_of('\0')));
            std::tm* tm_ptr = std::gmtime(&post.get_update_time());
            std::stringstream ss;
            ss<<std::put_time(tm_ptr, "%Y-%m-%d %H:%M");
            pt.put("update_date", ss.str());

            res.result(http::status::ok);

            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /post/register
    handler.insert({"/comment/insert",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::post){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            std::string token = req.at(http::field::authorization).to_string();
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }

            logic::CommentLogic* comment_logic = logic::CommentLogicImpl::getInstance();
            
            std::stringstream req_stream(req.body());
            boost::property_tree::read_json(req_stream, pt);

            domain::Comment comment;
            comment.get_post_id() = pt.get<uint32_t>("post_id");
            comment.get_content() = pt.get<std::string>("content");
            comment.get_is_valid() = true;

            int ret = comment_logic->add_comment(comment, token);
            if(ret==-1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret) {
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "interal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            pt.put("status", "success");
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /post/modify
    handler.insert({"/comment/modify",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::post){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            std::string token = req.at(http::field::authorization).to_string();
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }

            logic::CommentLogic* comment_logic = logic::CommentLogicImpl::getInstance();
            
            std::stringstream req_stream(req.body());
            boost::property_tree::read_json(req_stream, pt);

            domain::Comment comment;
            comment.get_id() = pt.get<uint32_t>("id");
            comment.get_post_id() = pt.get<uint32_t>("post_id");
            comment.get_content() = pt.get<std::string>("content");
            comment.get_is_valid() = true;

            int ret = comment_logic->modify_comment(comment, token);
            if(ret==-1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret) {
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "interal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            pt.put("status", "success");
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /comment/delete
    handler.insert({"/comment/delete",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::post){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            std::string token = req.at(http::field::authorization).to_string();
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }

            logic::CommentLogic* comment_logic = logic::CommentLogicImpl::getInstance();
            
            std::stringstream req_stream(req.body());
            boost::property_tree::read_json(req_stream, pt);

            uint32_t id = pt.get<uint32_t>("id");

            int ret = comment_logic->delete_comment(id,token);
            if(ret==-1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret) {
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "interal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);
            pt.put("status", "success");
            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();
            return;
        }
    });
    // /comment?post_id={id}
    handler.insert({"/comment",
        [&](const http::request<http::string_body>& req,http::response<http::string_body>& res){
            
            std::string path(req.target());
            std::string method(req.method_string());

            spdlog::info("{} {}", method, path);
            boost::property_tree::ptree pt;
            
            std::stringstream res_stream;

            res.set(http::field::content_type, "application/json");
            if(req.method() != http::verb::get){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not exist path");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            std::string token = req.at(http::field::authorization).to_string();
            if(token.empty()){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "token is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;                
            }

            logic::CommentLogic* comment_logic = logic::CommentLogicImpl::getInstance();
        
            std::string query_param = req.target().to_string();
            size_t id_pos = query_param.find("post_id");
            if(id_pos==std::string::npos){
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "id is empty");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            size_t start_pos = id_pos+8;
            size_t end_pos = query_param.find('&', start_pos);
            uint32_t id = static_cast<uint32_t>(std::stoul(query_param.substr(start_pos, end_pos)));

            std::vector<domain::Comment> comment_list;

            int ret = comment_logic->get_commentlist(&comment_list,id, token);
            if(ret==-1) {
                pt.clear();
                res.result(http::status::bad_request);
                pt.put("message", "not login");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            else if(ret) {
                pt.clear();
                res.result(http::status::internal_server_error);
                pt.put("message", "interal server error");
                boost::property_tree::write_json(res_stream,pt);
                res.body() = res_stream.str();
                return;
            }
            pt.clear();
            res.result(http::status::ok);

            boost::property_tree::ptree comment_tree;
            for(auto& comment: comment_list){
                boost::property_tree::ptree comment_element;
                comment_element.put("id", std::to_string(comment.get_id()));
                comment_element.put("post_id", std::to_string(comment.get_post_id()));
                comment_element.put("author", comment.get_author().substr(0, comment.get_author().find_first_of('\0')));
                comment_element.put("content", comment.get_content().substr(0, comment.get_content().find_first_of('\0')));
                comment_tree.push_back({"", comment_element});
            }
            pt.add_child("comments", comment_tree);

            boost::property_tree::write_json(res_stream,pt);
            res.body() = res_stream.str();

            return;
        }
    });
}
