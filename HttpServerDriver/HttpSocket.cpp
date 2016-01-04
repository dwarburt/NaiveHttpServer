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

        Socket::Socket(boost::asio::ip::tcp::socket socket, RequestHandler h, std::function<void(SocketPtr)> on_close, std::map<std::string,std::string> fsmap) 
            : m_socket(std::move(socket)), m_handler(h), m_on_close(on_close), buffer(8096), m_fsmap(fsmap)
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
        std::shared_ptr<Response> Socket::get_file_response(Request req)
        {
            std::string requested_url_path(req.get_url().get_path());
            auto ret = std::shared_ptr<Response>(nullptr);
            for (auto m : m_fsmap)
            {
                
                std::string url_path = m.first;
                std::string fspath = m.second;
                std::string request_path = req.get_url().get_path();
                if (url_path.size() > request_path.size())
                {
                    continue;
                }
                auto t = std::mismatch(url_path.begin(), url_path.end(), requested_url_path.begin());
                if (t.first == url_path.end())
                {
                    std::string newpath(fspath);
                    newpath.append(t.second, requested_url_path.end());
                    ret = std::shared_ptr<Response>(new Response());
                    ret->set_file_response(newpath);
                }
            }
            return ret;
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
                    auto file_response = get_file_response(req);
                    if (file_response)
                    {
                        resp = *file_response;
                    }
                    else
                    {
                       resp = m_handler(req);
                    }
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
                    error_code ignore;
                    m_socket.shutdown(tcp::socket::shutdown_both,
                        ignore);
                }
                delete data_buffer;
            });
        }
    }
}
