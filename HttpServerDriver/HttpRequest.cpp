#include "stdafx.h"
#include "HttpRequest.hpp"

#include <iostream>
#include <string>

namespace Naive
{
    namespace Http
    {

        Request::Request(std::vector<uint8_t> data, size_t data_size)
        {
            std::string requeststr(data.begin(), data.begin() + data_size);
            std::cout << "Got Request: " << requeststr << std::endl;
        }


        Request::~Request()
        {
        }
    } //namespace Http
} //namespace Naive
