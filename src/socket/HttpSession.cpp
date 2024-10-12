#include "socket/Http.hpp"
#include "spdlog/spdlog.h"
#include <utility>

HttpSession::HttpSession(tcp::socket &&socket, std::unordered_map<std::string, http_handler_t>& handler)
    : _socket(socket), _handler(handler) 
{   }

void HttpSession::run()
{
    do_read();
}

void HttpSession::do_read()
{
    auto self = shared_from_this();

    beast::error_code ec;
    http::read(self->socket(), _buffer, _request, ec);
    if(!ec.failed()) {
        self->handle_request();
    }
    else {
        spdlog::info("{} {}", ec.value(), ec.message());
    }
}

tcp::socket& HttpSession::socket()
{
    return _socket;
}


void HttpSession::handle_request()
{

    std::string path(_request.target());
    std::string method(_request.method_string());

    http::response<http::string_body> res;

    auto it = _handler.find(path);
    spdlog::info("{} {}", method, path);
    if(it != _handler.end()){
        it->second(_request, res);
    } else {
        //  handler가 없는 경우
        res.result(http::status::internal_server_error);
        res.version(11);
        res.set(http::field::server, "Boost.Beast");
        res.set(http::field::content_type, "application/json");
        res.body() = "Interal Server Error";
        res.prepare_payload();
    }
    auto self = shared_from_this();
    
    if(self->socket().is_open()){
        spdlog::info("{} {} {}", method, path, res.result_int());
        http::write(self->socket(), res);
        self->socket().shutdown(tcp::socket::shutdown_send);
    }
}
