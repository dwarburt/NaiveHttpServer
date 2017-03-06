#include <iostream>
#include <sstream>
#include <deque>
#include <algorithm>
#include "request.hpp"
#include "util.hpp"

namespace Naive
{
    namespace Http
    {
        Auth Request::authdata() const {
            return m_auth_data;
        }

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
            if (hline.size() < 3)
            {
                return false;
            }
            m_method = hline[0];
            std::transform(m_method.begin(), m_method.end(), m_method.begin(), ::toupper);
            m_url.parse(hline[1]);
            m_http_version = hline[2];

            while (v.size() > 0)
            {
                std::string x = v.front();
                v.pop_front();
                if (x == "")
                {
                    break;
                }
                std::vector<std::string> header;
                trimsplit(header, x, ":");
                m_headers[header[0]] = header[1];
                if (header[0] == "Authorization") {
                    m_auth_data = Auth::from_credentials(m_method, header[1]);
                }
            }
            //remaining strings in v are the request body
            m_body = join(v, "\r\n");
            return true;
        }
        bool Request::is_valid(AuthScheme scheme, Credentials creds) {
            if (m_auth_data.m_method == "invalid") {
                return false;
            }
            return m_auth_data.is_valid(scheme, creds);
        }

        Request::~Request()
        {
        }
        std::string Request::first_line()
        {
            return m_method + " " + m_url.to_string() +" " + m_http_version;
        }


    } //namespace Http
} //namespace Naive
