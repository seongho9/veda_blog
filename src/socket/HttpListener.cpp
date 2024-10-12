#include "socket/Http.hpp"
#include "spdlog/spdlog.h"

HttpListener::HttpListener(net::io_context& ctx, tcp::endpoint endpoint, std::unordered_map<std::string, http_handler_t>& handler)
    :_context(ctx), _acceptor(net::make_strand(ctx)), _handler(handler)
{
    beast::error_code ec;


    _acceptor.open(endpoint.protocol(), ec);
    _acceptor.set_option(net::socket_base::reuse_address(true), ec);

    _acceptor.bind(endpoint, ec);
    _acceptor.listen(net::socket_base::max_listen_connections, ec);
    spdlog::info("{}:{}", endpoint.address().to_string(), endpoint.port());
    
}

void HttpListener::add_handler(std::string path, http_handler_t handler)
{
    _handler.insert({path, handler});
}

void HttpListener::do_accpet()
{
    _acceptor.async_accept(
        net::make_strand(_context),
        [self=shared_from_this()](beast::error_code ec, tcp::socket socket)
        {
            spdlog::info("packet in");
            if(!ec){
                std::thread session_thread(
                    &HttpListener::handle_session, 
                    self, std::move(socket)
                );
                session_thread.detach();
            }
            self->do_accpet();
        });
}

void HttpListener::handle_session(tcp::socket&& socket)
{
    spdlog::info("{}", socket.remote_endpoint().address().to_string());
    std::shared_ptr<HttpSession> s_ptr = std::make_shared<HttpSession>(std::move(socket), _handler);
    s_ptr.get()->run();
}