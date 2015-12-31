#include "stdafx.h"
#include "HttpServer.hpp"
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace std::placeholders;

namespace Naive
{
    namespace Http
    {

        Server::Server() :
            m_io(),
            m_acceptor(m_io),
            m_socket(m_io)
        {
            std::string address = "127.0.0.1";
            std::string port = "55055";
            tcp::endpoint the_end = *(tcp::resolver(m_io).resolve({ address, port }));
            m_acceptor.open(the_end.protocol());
            m_acceptor.set_option(tcp::acceptor::reuse_address(true));
            m_acceptor.bind(the_end);
            m_acceptor.listen();

            wait_for_connection();
        }
        Server::~Server()
        {
        }
        void Server::start(RequestHandler handler)
        {
            m_handler = handler;
            m_io.run();
        }
        void Server::wait_for_connection()
        {
            debug("Waiting for connection");
            m_acceptor.async_accept(m_socket, std::bind(&Server::handle_connection, this, _1));
        }
        void Server::handle_connection(error_code error_code)
        {
            if (!m_acceptor.is_open())
            {
                return;
            }

            if (!error_code)
            {
                debug("Handling the connection");
                std::vector<uint8_t> buffer;
                m_socket.async_read_some(boost::asio::buffer(buffer), std::bind(&Server::got_data, this, buffer, _1, _2));
            }
            else
            {
                debug("on handle connection, got error code: " + error_code.message());
            }
            wait_for_connection();
        }
        void Server::got_data(std::vector<uint8_t> data, error_code ec, std::size_t bytes)
        {
            if (!ec)
            {
                Request req;
                Response resp = m_handler(req);
                respond(resp.getCode(), resp.getText());
            }
            else if (ec != error::operation_aborted)
            {
                close_connection();
            }
        }
        void Server::respond(uint8_t code, std::string response)
        {
            std::string status = "HTTP/1.1 200 OK\r\n";
            std::string type = "Content-Type: text/plain\r\n";
            std::string len = "Content-Length: " + std::to_string(response.length()) + "\r\n";
            std::string sep = "\r\n";
            std::string reply = status + type + len + sep + response;
            std::string *data_buffer = new std::string(reply);

            async_write(m_socket, boost::asio::buffer(*data_buffer),
                [this, data_buffer](error_code ec, std::size_t)
            {
                if (!ec)
                {
                    // Initiate graceful connection closure.
                    error_code ignored_ec;
                    m_socket.shutdown(tcp::socket::shutdown_both,
                        ignored_ec);
                }

                if (ec != error::operation_aborted)
                {
                    close_connection();
                }
                delete data_buffer;
            });
        }
        void Server::close_connection()
        {
            debug("Closing the connection");
            m_socket.close();
        }
        void Server::debug(std::string msg)
        {
            std::cout << msg << std::endl;
        }
    } //namespace Http
} //namespace Naive
