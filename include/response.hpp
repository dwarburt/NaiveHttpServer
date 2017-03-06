#pragma once
#include <map>
#include <memory>
#include "message.hpp"
#include "request.hpp"

namespace Naive
{
    namespace Http
    {
        typedef std::vector<Credentials> CredentialList;
        class Response : public Message
        {
        public:
            Response();
            Response(std::string response_text, uint16_t http_code = 200);
            ~Response();


            std::string get_text();
            uint16_t get_code();
            void set_code(uint16_t);
            void set_header(std::string, std::string);
            void set_response_body(std::string);
            void set_file_response(std::string fpath);
            void set_authenticate_header(AuthScheme scheme);

            static RequestHandler blank(uint16_t code);
            static RequestHandler simple(const std::string response, uint16_t code = 200);
            static RequestHandler protect(AuthScheme scheme,
              Credentials acceptable,
              FinalRequestHandler next);
            static RequestHandler protect(AuthScheme scheme,
              std::function<Credentials()> credential_resolver,
              FinalRequestHandler next);
            static RequestHandler protect(AuthScheme scheme,
              std::function<CredentialList()> credentials_resolver,
              FinalRequestHandler next);

        private:
            std::string m_body;
            uint16_t m_code;
            void default_headers();
            static std::map<uint8_t, std::string> http_codes;
            static std::map<std::string, std::string> mime_types;
            std::string create_digest_challenge();

        protected:
            virtual std::string first_line();
        };
    } //namespace Http
} //namespace Server
