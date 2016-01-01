#include <iostream>
#include <string>
#include <sstream>

#include <boost/algorithm/string/split.hpp>

#include "HttpRequest.hpp"

namespace Naive
{
    namespace Http
    {

        Request::Request(std::vector<uint8_t> data, size_t data_size)
        {
            std::string requeststr(data.begin(), data.begin() + data_size);
            std::cout << "Got Request: " << requeststr << std::endl;

            //parse
            std::istringstream ss(requeststr, std::ios_base::in);
        }


        Request::~Request()
        {
        }
        bool Request::keep_alive()
        {
            return false;
        }
    } //namespace Http
} //namespace Naive
