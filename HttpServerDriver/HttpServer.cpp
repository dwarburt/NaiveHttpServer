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
            m_acceptor(m_io)
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
            m_psocket = std::shared_ptr<tcp::socket>(new tcp::socket(m_io));
            debug("Waiting for connection");
            m_acceptor.async_accept(*m_psocket, std::bind(&Server::handle_connection, this, _1));
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
                SocketPtr ns = std::make_shared<Socket>(std::move(*m_psocket), m_handler);
                m_socket_list.insert(ns);
                ns->handle();

            }
            else
            {
                debug("on handle connection, got error code: " + error_code.message());
            }
            wait_for_connection();
        }


        void Server::close_socket(SocketPtr sp)
        {
            debug("Closing the connection");
            m_socket_list.erase(sp);
            
        }
        void Server::debug(std::string msg)
        {
            std::cout << msg << std::endl;
        }
    } //namespace Http
} //namespace Naive
