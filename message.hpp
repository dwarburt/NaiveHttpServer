#pragma once
#include <string>
#include <map>
#include "url.hpp"

namespace Naive
{
    namespace Http
    {
        class Message
        {
        public:
            Message();
            virtual ~Message();
            std::string to_string();
            bool keep_alive();
            std::string get_header(std::string);
            std::string get_method();
            Url get_url();
            std::string get_http_version();
            std::string get_body();
            virtual void set_body(std::string);
            std::map<std::string, std::string> get_headers();

        protected:
            virtual std::string first_line() = 0;
            std::map<std::string, std::string> m_headers;
            std::string m_method;  // one of GET, POST etc.
            Url m_url;
            std::string m_http_version;
            std::string m_body;
        };
    }
}
