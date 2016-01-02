#pragma once
#include <map>
#include "HttpMessage.hpp"

namespace Naive
{
    namespace Http
    {
        class Response : public Message
        {
        public:
            Response();
            Response(std::string response_text, uint8_t http_code = 200);
            ~Response();

            std::string getText();
            uint8_t getCode();

        private:
            std::string m_text;
            uint8_t m_code;

            static std::map<uint8_t, std::string> http_codes;
        };
    } //namespace Http
} //namespace Server
