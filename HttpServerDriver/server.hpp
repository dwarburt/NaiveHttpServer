#pragma once
#include <set>
#include <boost/asio.hpp>
#include "request.hpp"
#include "response.hpp"
#include "socket.hpp"
#include "util.hpp"

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
            void route_to_files(std::string, std::string);
        private:
            void wait_for_connection();
            void handle_connection(boost::system::error_code error_code);
            void close_socket(SocketPtr);

            std::map<std::string, std::string> m_routes_to_files;
            boost::asio::io_service m_io;
            boost::asio::ip::tcp::acceptor m_acceptor;
            std::shared_ptr<boost::asio::ip::tcp::socket> m_psocket;
            RequestHandler m_handler;
            std::set<SocketPtr> m_socket_list;

        };
    } // namespace Http
} // namespace Server
