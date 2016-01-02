#include <iostream>
#include <string>
#include <sstream>
#include <deque>

#include "HttpRequest.hpp"
#include "HttpUtil.hpp"

namespace Naive
{
    namespace Http
    {

        bool Request::parse(std::vector<uint8_t> data, size_t data_size)
        {
            if (data_size == 0)
            {
                return false;
            }
            std::string requeststr(data.begin(), data.begin() + data_size);

            std::deque<std::string> v;
            std::deque<std::string> hline;
            split(v, requeststr, "\r\n");
            
            std::string http_line = v.front();
            v.pop_front();
            split(hline, http_line, " ");
            m_method = hline[0];
            m_url = hline[1];
            m_http_version = hline[2];

            while (v.size() > 0)
            {
                std::string x = v.front();
                v.pop_front();
                if (x == "")
                {
                    break;
                }
                std::cout << "header: " << x << std::endl;
                std::vector<std::string> header;
                split(header, x, ": ");
                m_headers[header[0]] = header[1];
            }
            //remaining strings in v are the request body
            m_body = join(v, "\r\n");
            return true;
        }


        Request::~Request()
        {
        }


    } //namespace Http
} //namespace Naive
