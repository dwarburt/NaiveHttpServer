#pragma once
#include <asio.hpp>
#include "request.hpp"
#include "response.hpp"

namespace Naive
{
    namespace Http
    {
        class Socket;
        typedef std::shared_ptr<Socket> SocketPtr;
        // The purpose of this class is to manage the lifetime of a tcp::socket, read and write from
        // that socket and close it when done.
        class Socket : public std::enable_shared_from_this<Socket>
        {
        public:
            Socket(
                asio::ip::tcp::socket socket_on_which_to_communicate,
                RequestHandler callback_to_userspace_for_handling_requests,
                std::function<void(SocketPtr)> callback_to_notify_when_socket_can_close,
                std::map<std::string,std::string> map_of_routes_to_filesystem_directories
                );
            ~Socket();
            void handle();
            void close();

        private:
            asio::ip::tcp::socket m_socket;
            RequestHandler m_handler;
            std::function<void(SocketPtr)> m_on_close;
            std::vector<uint8_t> buffer;
            std::map<std::string, std::string> m_fsmap;

            void got_data(asio::error_code ec, std::size_t bytes);
            void respond(uint8_t code, std::string response_text);
            std::shared_ptr<Response> get_file_response(Request);
        };


    }
}
