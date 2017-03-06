#pragma once
#include <set>
#include <asio.hpp>
#include <deque>
#include <thread>
#include <unordered_map>
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
            Server(const std::string &bind_address = "0.0.0.0", uint16_t port = 55055);
            ~Server();

            void start();
            MiddlewareHandle prepend(RequestHandler);
            MiddlewareHandle append(RequestHandler);
            void remove(MiddlewareHandle h);
            void route_to_files(std::string, std::string);
            void get(const std::string path, FinalRequestHandler);
            void post(const std::string path, FinalRequestHandler);
        private:
            struct MiddlewareEntry {
                MiddlewareHandle handle;
                RequestHandler handler;
            };
            void wait_for_connection();
            void handle_connection(asio::error_code error_code);
            void close_socket(SocketPtr);
            bool is_middleware(MiddlewareHandle h);
            MiddlewareHandle new_handle();
            HandleResult handle_request(RequestPtr, ResponsePtr);

            std::map<std::string, std::string> m_routes_to_files;
            asio::io_service m_io;
            asio::ip::tcp::acceptor m_acceptor;
            std::shared_ptr<asio::ip::tcp::socket> m_psocket;
            std::deque<MiddlewareEntry> m_handlers;
            std::set<MiddlewareHandle> m_known_middlewares;
            std::set<SocketPtr> m_socket_list;
            std::shared_ptr<std::thread> m_io_thread;

            typedef std::unordered_map<std::string, MiddlewareHandle> FinalHandlers;
            FinalHandlers m_get_handlers, m_post_handlers;

        };
    } // namespace Http
} // namespace Server
