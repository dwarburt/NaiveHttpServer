#pragma once
#include <vector>
#include <map>
#include <memory>
#include <type_traits>
#include "message.hpp"

namespace Naive
{
    namespace Http
    {
        class Request : public Message
        {
        public:
            Request() : m_auth_data("invalid") {}
            virtual ~Request();
            bool parse(std::vector<uint8_t>, size_t);

            // specialize the behavior of the parent class by not allowing the
            // the body to be set in a Request.
            template<typename T = bool>
            void set_body(std::string)
            {
                static_assert(std::is_same<T, std::false_type>::value, "Cannot set body of the request");
            }
            std::string path() {
              return get_url().get_path();
            }
            bool is_path(const std::string &test_path) {
              return path() == test_path;
            }
            bool is_valid(AuthScheme scheme, Credentials creds);
            std::map<std::string, std::string> data;
            Auth authdata() const;
        private:
            Auth m_auth_data;

        protected:
            virtual std::string first_line();

        };
    } //namespace Http
} //namespace Naive
