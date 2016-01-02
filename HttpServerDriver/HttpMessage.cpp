#include "HttpMessage.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace Naive
{
    namespace Http
    {
        Message::Message()
        {
        }

        Message::~Message()
        {
        }
        std::string Message::to_string()
        {
            std::ostringstream oss;
            oss << m_method << " " << m_url << " " << m_http_version << "\r\n";
            std::for_each(m_headers.begin(), m_headers.end(),
                [&oss](std::pair<const std::string, std::string> &x) { oss << x.first << ": " << x.second << "\r\n"; });
            oss << "\r\n" << m_body;
            return oss.str();
        }
        std::map<std::string, std::string> Message::get_headers()
        {
            return m_headers;
        }
        bool Message::keep_alive()
        {
            if (get_header("Connection") == "keep-alive")
            {
                return true;
            }
            return false;
        }
        std::string Message::get_header(std::string name)
        {
            if (m_headers.find(name) != m_headers.end())
            {
                return m_headers[name];
            }
            return "";

        }
        std::string Message::get_operation()
        {
            return m_method;
        }
        std::string Message::get_url()
        {
            return m_url;
        }
        std::string Message::get_http_version()
        {
            return m_http_version;
        }
        std::string Message::get_body()
        {
            return m_body;
        }
    }
}
