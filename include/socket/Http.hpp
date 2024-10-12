#ifndef _HTTP_H
#define _HTTP_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include <boost/config.hpp>

#include<iostream>
#include<string>
#include<thread>
#include<unordered_map>
#include<deque>


namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using tcp=net::ip::tcp;
using http_handler_t = 
    std::function<void(
        const http::request<http::string_body>& req,
        http::response<http::string_body>& res)>;

class HttpSession : public std::enable_shared_from_this<HttpSession>
{
    tcp::socket& _socket;
    beast::flat_buffer _buffer;
    http::request<http::string_body> _request;

    std::unordered_map<std::string, http_handler_t>& _handler;

public:
    HttpSession() = default;
    HttpSession(tcp::socket &&socket, std::unordered_map<std::string, http_handler_t>& handler);

    tcp::socket& socket();
    void run();

private:
    void do_read();
    void handle_request();
};

class HttpListener : public std::enable_shared_from_this<HttpListener>
{
    net::io_context& _context;
    tcp::acceptor _acceptor;

    std::unordered_map<std::string, http_handler_t> _handler;
    

public:
    HttpListener() = default;
    HttpListener(net::io_context& ctx, tcp::endpoint endpoint, std::unordered_map<std::string, http_handler_t>& handler);

    void add_handler(std::string path, http_handler_t handler);
    void do_accpet();

    void handle_session(tcp::socket&& socket);
private:
};

class HttpStarter
{
public:
    HttpStarter() = default;

    void run(std::string addr, std::string port);
    void register_handler(std::unordered_map<std::string, http_handler_t>& handler);
};

#endif