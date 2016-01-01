#include <iostream>
#include <string>
#include <sstream>
#include <deque>

#include <boost/algorithm/string/iter_find.hpp>
#include <boost/algorithm/string/finder.hpp>

#include "HttpRequest.hpp"

namespace Naive
{
    namespace Http
    {

        Request::Request(std::vector<uint8_t> data, size_t data_size)
        {
            std::string requeststr(data.begin(), data.begin() + data_size);
            std::cout << "Got Request: " << requeststr << std::endl;

            std::deque<std::string> v;
            iter_split(v, requeststr, boost::algorithm::first_finder("\r\n"));
            
            std::string http_line = v.front();
            v.pop_front();
            while (v.size() > 0)
            {
                std::string x = v.front();
                v.pop_front();
                if (x == "")
                {
                    break;
                }
                std::cout << "header: " << x << std::endl;
            }
            //remaining strings in v are the request body
            
            std::stringstream bodyss;
            for (size_t i = 0; i < v.size(); ++i)
            {
                if (i != 0)
                    bodyss << "\r\n";
                bodyss << v[i];
            }
            std::string body = bodyss.str();
            
        }


        Request::~Request()
        {
        }
        bool Request::keep_alive()
        {
            return false;
        }
        std::string Request::get_header(std::string name)
        {
            if (m_headers.find(name) != m_headers.end())
            {
                return m_headers[name];
            }
            return "";
            
        }
        std::string Request::get_operation()
        {
            return m_operation;
        }
        std::string Request::get_url()
        {
            return m_url;
        }
        std::string Request::get_http_version()
        {
            return m_http_version;
        }
    } //namespace Http
} //namespace Naive
