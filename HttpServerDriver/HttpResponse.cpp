#include "HttpResponse.hpp"

namespace Naive
{
    namespace Http
    {
        std::map<uint8_t, std::string> Response::http_codes{
            { 200, "OK" },
            { 301, "Moved Permanently" },
            { 400, "Bad Request" },
            { 401, "Not Authorized" },
            { 403, "Forbidden" },
            { 404, "Not Found" },
            { 500, "Internal Server Error" }
        };
        Response::Response() : m_body("invalid"), m_code(400)
        {
            default_headers();
        }
        Response::Response(std::string response_text, uint8_t http_code) :
            m_code(http_code)
        {
            set_body(response_text);
            default_headers();
        }
        void Response::default_headers()
        {
            set_header("Connection", "keep-alive");
            set_header("Content-Type", "text/plain");
        }
        Response::~Response()
        {
        }
        void Response::set_body(std::string b)
        {
            m_body = b;
            set_header("Content-Length", std::to_string(m_body.size()));
        }
        std::string Response::get_text()
        {
            return m_body;
        }
        std::string Response::first_line()
        {
            return "HTTP/1.1 200 OK";
        }
        uint8_t Response::get_code()
        {
            return m_code;
        }

        void Response::set_header(std::string key, std::string value)
        {
            m_headers[key] = value;
        }

    } //namespace Http
} //namespace Naive
