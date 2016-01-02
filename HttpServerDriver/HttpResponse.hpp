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

            std::string get_text();
            uint8_t get_code();
            void set_header(std::string, std::string);
            void set_response_body(std::string);

        private:
            std::string m_body;
            uint8_t m_code;
            void default_headers();
            static std::map<uint8_t, std::string> http_codes;
        protected:
            virtual std::string first_line();

            
        };
    } //namespace Http
} //namespace Server
