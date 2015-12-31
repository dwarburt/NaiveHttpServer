#pragma once
#include <boost/asio.hpp>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "HttpSocket.hpp"

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
            void respond(uint8_t code, std::string response);
            void close_connection();
            void debug(std::string msg);
            void Server::got_data(std::vector<uint8_t> data, boost::system::error_code ec, std::size_t bytes);

            boost::asio::io_service m_io;
            boost::asio::ip::tcp::acceptor m_acceptor;
            boost::asio::ip::tcp::socket m_socket;
            RequestHandler m_handler;

        };
    } // namespace Http
} // namespace Server


