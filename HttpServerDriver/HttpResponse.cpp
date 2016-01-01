#include "HttpResponse.hpp"

namespace Naive
{
    namespace Http
    {
        std::map<uint8_t, std::string> Response::http_codes{
            { 200, "OK" },
            { 301, "Moved Permanently" },
            { 400, "Bad Request" },
            { 401, "Not Authorized" },
            { 403, "Forbidden" },
            { 404, "Not Found" },
            { 500, "Internal Server Error" }
        };

        Response::Response(std::string response_text, uint8_t http_code) :
            m_text(response_text),
            m_code(http_code)
        {
        }

        Response::~Response()
        {
        }

        std::string Response::getText()
        {
            return m_text;
        }

        uint8_t Response::getCode()
        {
            return m_code;
        }

    } //namespace Http
} //namespace Naive
