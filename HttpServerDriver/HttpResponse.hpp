#pragma once
#include <map>

namespace Naive
{
    namespace Http
    {
        class Response
        {
        public:
            Response();
            ~Response();

            static std::map<uint8_t, std::string> http_codes;
        };
    } //namespace Http
} //namespace Server
