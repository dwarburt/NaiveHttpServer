#pragma once
#include <vector>
#include <map>

#include "HttpMessage.hpp"

namespace Naive
{
    namespace Http
    {
        class Request : public Message
        {
        public:
            ~Request();
            bool parse(std::vector<uint8_t>, size_t);


        private:
        protected:
            virtual std::string first_line();

        };
    } //namespace Http
} //namespace Naive
