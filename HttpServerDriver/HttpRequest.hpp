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

            bool keep_alive();
        };
    } //namespace Http
} //namespace Naive
