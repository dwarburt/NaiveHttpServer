#include "server.hpp"

using namespace asio;
using namespace asio::ip;
using namespace std::placeholders;

namespace Naive
{
    namespace Http
    {

        Server::Server(const std::string &bind_address, uint16_t port) :
            m_io(),
            m_acceptor(m_io)
        {
            std::string bind_port = to_string(port);
            tcp::endpoint the_end = *(tcp::resolver(m_io).resolve({ bind_address, bind_port }));
            m_acceptor.open(the_end.protocol());
            m_acceptor.set_option(tcp::acceptor::reuse_address(true));
            m_acceptor.bind(the_end);
            m_acceptor.listen();

            wait_for_connection();
        }
        Server::~Server()
        {
            if (m_io_thread != nullptr)
            {
                m_io.stop();
                m_io_thread->join();
            }
        }
        void Server::start()
        {
            m_io_thread = std::shared_ptr<std::thread>(
                new std::thread([this]() {
                    m_io.run();
                })
            );
        }
        HandleResult Server::handle_request(RequestPtr req, ResponsePtr resp)
        {
            int i = 0;
            auto result = Continue;
            for (const auto &mw : m_handlers)
            {
                i++;
                result = mw.handler(req, resp);
                if (result == Stop) {
                    break;
                }
            }
            if (result == Continue) {
                //no-one wanted to be the final handler so send a 404
                resp->set_code(404);
            }
            return Stop;
        }
        bool Server::is_middleware(MiddlewareHandle h)
        {
            return m_known_middlewares.find(h) != m_known_middlewares.end();
        }
        MiddlewareHandle Server::new_handle()
        {
            MiddlewareHandle h;
            do {
                auto bb = random_bytes(4);
                h =   (bb[3] << 3)
                    | (bb[2] << 2)
                    | (bb[1] << 1)
                    | (bb[0]     );

            } while (is_middleware(h));
            m_known_middlewares.insert(h);
            return h;
        }
        MiddlewareHandle Server::append(RequestHandler handler)
        {
            auto h = new_handle();
            m_handlers.push_back({h, handler});
            return h;
        }
        MiddlewareHandle Server::prepend(RequestHandler handler)
        {
            auto h = new_handle();
            m_handlers.push_front({h, handler});
            return h;
        }
        void Server::remove(MiddlewareHandle h)
        {
            if (!is_middleware(h)) {
                return;
            }
            for (auto mw = m_handlers.begin(); mw != m_handlers.end(); mw++) {
                if (mw->handle == h) {
                    m_handlers.erase(mw);
                    break;
                }
            }
            m_known_middlewares.erase(h);
        }
        void Server::route_to_files(std::string route, std::string fs_path)
        {
            if (route[route.size() - 1] != '/')
            {
                route.append(1, '/');
            }
            if (fs_path[fs_path.size() - 1] != '/')
            {
                fs_path.append(1, '/');
            }
            m_routes_to_files[route] = fs_path;
        }
        void Server::wait_for_connection()
        {
            m_psocket = std::shared_ptr<tcp::socket>(new tcp::socket(m_io));
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
                SocketPtr ns = std::make_shared<Socket>(
                    std::move(*m_psocket),
                    std::bind(&Server::handle_request, this, _1, _2),
                    std::bind(&Server::close_socket, this, _1),
                    m_routes_to_files
                    );
                m_socket_list.insert(ns);
                ns->handle();
            }
            else
            {
                // TODO: log something
            }
            wait_for_connection();
        }


        void Server::close_socket(SocketPtr sp)
        {
            m_socket_list.erase(sp);

        }
        void Server::get(const std::string path, FinalRequestHandler clientHandler)
        {
            auto existing_final_handler = m_get_handlers.find(path);
            if (existing_final_handler != m_get_handlers.end()) {
                remove(existing_final_handler->second);

            }
            auto handler = [path, clientHandler](RequestPtr req, ResponsePtr resp) {
                if (req->get_method() == "GET" && req->get_url().get_path() == path) {
                    clientHandler(req, resp);
                    return Stop;
                }
                return Continue;
            };
            m_get_handlers[path] = append(handler);
        }
        void Server::post(const std::string path, FinalRequestHandler clientHandler)
        {
            auto existing_final_handler = m_post_handlers.find(path);
            if (existing_final_handler != m_post_handlers.end()) {
                remove(existing_final_handler->second);
            }
            auto handler = [path, clientHandler](RequestPtr req, ResponsePtr resp) {
                if (req->get_method() == "POST" && req->get_url().get_path() == path) {
                    clientHandler(req, resp);
                    return Stop;
                }
                return Continue;
            };
            m_post_handlers[path] = append(handler);
        }
    } //namespace Http
} //namespace Naive
