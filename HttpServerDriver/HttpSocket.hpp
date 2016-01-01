#pragma once
#include <boost/asio.hpp>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

namespace Naive
{
    namespace Http
    {
        class Socket;
        typedef std::function<Response(Request)> RequestHandler;
        typedef std::shared_ptr<Socket> SocketPtr;
        // The purpose of this class is to manage the lifetime of a tcp::socket, read and write from
        // that socket and close it when done.
        class Socket : public std::enable_shared_from_this<Socket>
        {
        public:
            Socket(boost::asio::ip::tcp::socket, RequestHandler, std::function<void(SocketPtr)>);
            ~Socket();
            void handle();
            void close();

        private:
            boost::asio::ip::tcp::socket m_socket;
            RequestHandler m_handler;
            std::function<void(SocketPtr)> m_on_close;
            std::vector<uint8_t> buffer;

            void got_data(boost::system::error_code ec, std::size_t bytes);
            void respond(uint8_t code, std::string response_text);
        };

        
    }
}

