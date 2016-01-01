#pragma once
#include <vector>
namespace Naive
{
    namespace Http
    {
        class Request
        {
        public:
            Request(std::vector<uint8_t>, size_t);
            ~Request();
        };
    } //namespace Http
} //namespace Naive
