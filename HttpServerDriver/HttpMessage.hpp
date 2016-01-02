#pragma once
#include <string>
#include <map>

namespace Naive
{
    namespace Http
    {
        class Message
        {
        public:
            Message();
            ~Message();
            std::string to_string();
            bool keep_alive();
            std::string get_header(std::string);
            std::string get_operation();
            std::string get_url();
            std::string get_http_version();
            std::string get_body();
            std::map<std::string, std::string> get_headers();

        protected:
            virtual std::string first_line() = 0;
            std::map<std::string, std::string> m_headers;
            std::string m_method;  // one of GET, POST etc.
            std::string m_url;
            std::string m_http_version;
            std::string m_body;
        };
    }
}
