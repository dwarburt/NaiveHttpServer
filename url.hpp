#pragma once
#include <string>
#include <map>
namespace Naive
{
    namespace Http
    {
        class Url
        {
        public:
            Url(std::string);
            Url();
            ~Url();

            std::string get_protocol();
            std::string get_host();
            std::string get_path();
            std::map<std::string, std::string> get_query();
            std::string to_string();

            void parse(std::string);
            static std::string percent_encode(const std::string&);
            static std::string percent_decode(const std::string&);
        private:
            std::string m_protocol;
            std::string m_host;
            std::string m_path;
            std::map<std::string, std::string> m_query;
        };
    }
}
