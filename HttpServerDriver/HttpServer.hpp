#pragma once
#include <boost/asio.hpp>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

namespace Naive
{
namespace Http
{

class Server
{
public:
    Server();
    ~Server();

    typedef std::function<Response(Request)> RequestHandler;

    void start(RequestHandler handler);
    static std::map<uint8_t, std::string> http_codes;

private:
    void wait_for_connection();
    void handle_connection();
    void respond(uint8_t code, std::string response);
    void close_connection();
    void debug(std::string msg);

    boost::asio::io_service m_io;
    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_socket;
    RequestHandler m_handler;

};
} // namespace Http
} // namespace Server


