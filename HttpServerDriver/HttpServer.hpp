#pragma once
#include <set>
#include <boost/asio.hpp>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpSocket.hpp"
#include "HttpUtil.hpp"

namespace Naive
{
    namespace Http
    {

        class Server
        {
        public:
            Server();
            ~Server();

            void start(RequestHandler handler);

        private:
            void wait_for_connection();
            void handle_connection(boost::system::error_code error_code);
            void close_socket(SocketPtr);

            boost::asio::io_service m_io;
            boost::asio::ip::tcp::acceptor m_acceptor;
            std::shared_ptr<boost::asio::ip::tcp::socket> m_psocket;
            RequestHandler m_handler;
            std::set<SocketPtr> m_socket_list;

        };
    } // namespace Http
} // namespace Server


