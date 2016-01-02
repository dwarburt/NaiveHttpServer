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

        };
    } //namespace Http
} //namespace Naive
