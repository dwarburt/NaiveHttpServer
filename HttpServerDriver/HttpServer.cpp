#include "stdafx.h"
#include "HttpServer.h"
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

std::map<uint8_t, std::string> HttpServer::http_codes {
    {200, "OK"},
    {301, "Moved Permanently"},
    {400, "Bad Request"},
    {401, "Not Authorized"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {500, "Internal Server Error"}
};

HttpServer::HttpServer() :
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


HttpServer::~HttpServer()
{
}

void HttpServer::start(RequestHandler handler)
{
    m_handler = handler;
    m_io.run();
}

void HttpServer::wait_for_connection()
{
    debug("Waiting for connection");
    m_acceptor.async_accept(m_socket, [this](boost::system::error_code error_code)
      {
          if (!m_acceptor.is_open())
          {
              return;
          }

          if (!error_code)
          {
              debug("Handling the connection");
              handle_connection();
          }

          wait_for_connection();
    });
}
void HttpServer::handle_connection()
{
    std::vector<uint8_t> buffer;
    m_socket.async_read_some(boost::asio::buffer(buffer), 
        [this](boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (!ec)
        {
            respond(200, "Hello world");
        }
        else if (ec != boost::asio::error::operation_aborted)
        {
            close_connection();
        }
    });
}
void HttpServer::respond(uint8_t code, std::string response)
{
    std::string status = "HTTP/1.1 200 OK\r\n";
    std::string type = "Content-Type: text/plain\r\n";
    std::string len = "Content-Length: " + std::to_string(response.length()) + "\r\n";
    std::string sep = "\r\n";
    std::string reply = status + type + len + sep + response;
    std::string *data_buffer = new std::string(reply);

    boost::asio::async_write(m_socket, boost::asio::buffer(*data_buffer),
        [this, data_buffer](boost::system::error_code ec, std::size_t)
    {
        if (!ec)
        {
            // Initiate graceful connection closure.
            boost::system::error_code ignored_ec;
            m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                ignored_ec);
        }

        if (ec != boost::asio::error::operation_aborted)
        {
            close_connection();
        }
        delete data_buffer;
    });
}
void HttpServer::close_connection()
{
    m_socket.close();
}
void HttpServer::debug(std::string msg)
{
    std::cout << msg << std::endl;
}
