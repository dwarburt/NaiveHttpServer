#include <iostream>
#include "HttpSocket.hpp"
#include "HttpUtil.hpp"
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;
using namespace std::placeholders;

namespace Naive
{
    namespace Http
    {

        Socket::Socket(boost::asio::ip::tcp::socket socket, RequestHandler h, std::function<void(SocketPtr)> on_close) 
            : m_socket(std::move(socket)), m_handler(h), m_on_close(on_close), buffer(8096)
        {
        }

        Socket::~Socket()
        {
            m_socket.close();
        }

        void Socket::handle()
        {
            m_socket.async_read_some(boost::asio::buffer(buffer), std::bind(&Socket::got_data, this, _1, _2));
        }

        void Socket::close()
        {
            SocketPtr p(shared_from_this());

            m_on_close(p);
        }
        void Socket::got_data(error_code ec, std::size_t bytes)
        {
            if (!ec)
            {
                Request req;
                Response resp;
                debug("Got data on this socket " + std::to_string(m_socket.native_handle()));

                if (req.parse(buffer, bytes))
                {
                    resp = m_handler(req);
                }

                buffer.clear();
                respond(resp.get_code(), resp.to_string());

                if (req.keep_alive())
                {
                    handle();
                }
                else
                {
                    close();
                }
           }
            else if (ec != error::operation_aborted)
            {
                close();
            }
        }

        void Socket::respond(uint8_t code, std::string response_text)
        {
            std::string *data_buffer = new std::string(response_text);

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
                delete data_buffer;
            });
        }
    }
}
