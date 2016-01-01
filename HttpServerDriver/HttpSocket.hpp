#pragma once
#include <boost/asio.hpp>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

namespace Naive
{
    namespace Http
    {
        typedef std::function<Response(Request)> RequestHandler;

        // The purpose of this class is to manage the lifetime of a tcp::socket, read and write from
        // that socket and close it when done.
        class Socket
        {
        public:
            Socket(boost::asio::ip::tcp::socket, RequestHandler);
            ~Socket();
            void handle();
            void close();

        private:
            boost::asio::ip::tcp::socket m_socket;
            RequestHandler m_handler;

            void got_data(std::vector<uint8_t> data, boost::system::error_code ec, std::size_t bytes);
            void respond(uint8_t code, std::string response_text);
        };

        typedef std::shared_ptr<Socket> SocketPtr;
    }
}

