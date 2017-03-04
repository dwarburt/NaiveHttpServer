#pragma once
#include <vector>
#include <map>

#include "message.hpp"

namespace Naive
{
    namespace Http
    {
        class Request : public Message
        {
        public:
            virtual ~Request();
            bool parse(std::vector<uint8_t>, size_t);

            template<typename T = bool>
            void set_body(std::string)
            {
                static_assert(std::false_type::value, "Cannot set body of the request");
            }

        private:
        protected:
            virtual std::string first_line();

        };
    } //namespace Http
} //namespace Naive
