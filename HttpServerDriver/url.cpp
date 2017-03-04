#include <vector>
#include <deque>
#include "url.hpp"
#include "util.hpp"


namespace Naive
{
    namespace Http
    {

        Url::Url(std::string u)
        {
            parse(u);
        }
        Url::Url()
        {

        }

        Url::~Url()
        {
        }

        void Url::parse(std::string u)
        {
            std::vector<std::string> v1;
            std::string remainder;
            split_first(v1, u, "://");
            if (v1.size() == 1)
            {
                remainder = v1.front();
            }
            else
            {
                m_protocol = v1.front();
                remainder = v1[1];
            }
            split_first(v1, remainder, "/");
            m_host = v1.front();
            if (v1.size() == 1)
            {
                return;
            }
            remainder = "/" + v1[1];
            split_first(v1, remainder, "?");
            m_path = percent_decode(v1.front());
            if (v1.size() == 1)
            {
                return;
            }
            remainder = v1[1];
            split(v1, remainder, "&");
            std::for_each(v1.begin(), v1.end(), [this](const std::string &x) {
                std::vector<std::string> param;
                split_first(param, x, "=");
                if (param.size() == 2)
                {
                    m_query[percent_decode(param[0])] = percent_decode(param[1]);
                }
                else
                {
                    m_query[percent_decode(param[0])] = "";
                }

            });
        }
        std::string Url::percent_encode(const std::string &)
        {
            return std::string();
        }
        std::string Url::percent_decode(const std::string &input)
        {
            std::ostringstream oss;
            for (int x = 0; x < input.size(); x++)
            {
                if (input[x] == '%')
                {
                    x++;
                    if (x < input.size())
                    {
                        std::string hex(input, x, 2);
                        char decoded_char = std::stoi(hex, 0, 16);
                        oss << std::string(1, decoded_char);
                    }
                    x++;
                }
                else if (input[x] == '+')
                {
                    oss << " ";
                }
                else
                {
                    oss << std::string(1, input[x]);
                }
            }
            return oss.str();
        }
        std::string Url::get_protocol()
        {
            return m_protocol;
        }
        std::string Url::get_host()
        {
            return m_host;
        }
        std::string Url::get_path()
        {
            return m_path;
        }
        std::map<std::string, std::string> Url::get_query()
        {
            return m_query;
        }
        std::string Url::to_string()
        {
            std::string ret( m_protocol + "://" + m_host);
            if (m_path != "")
            {
                ret += "/" + m_path;
            }
            if (m_query.size() > 0)
            {
                ret += "&";
                std::for_each(m_query.begin(), m_query.end(), [&ret](const std::pair<std::string, std::string> &qx) {
                    ret += std::string( qx.first + "=" + qx.second);
                });
            }
            return ret;
        }
    }
}
