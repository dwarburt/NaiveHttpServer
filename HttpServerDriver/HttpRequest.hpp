#pragma once
#include <vector>
#include <map>

namespace Naive
{
    namespace Http
    {
        class Request
        {
        public:
            Request(std::vector<uint8_t>, size_t);
            ~Request();

            bool keep_alive();
            std::string get_header(std::string);
            std::string get_operation();
            std::string get_url();
            std::string get_http_version();

        private:
            std::map<std::string, std::string> m_headers;
            std::string m_operation;  // one of GET, POST etc.
            std::string m_url;
            std::string m_http_version;
        };
    } //namespace Http
} //namespace Naive
