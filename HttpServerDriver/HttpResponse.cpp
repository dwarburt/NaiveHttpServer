#include "stdafx.h"
#include "HttpResponse.hpp"

namespace Naive
{
    namespace Http
    {


        std::map<uint8_t, std::string> Server::http_codes{
            { 200, "OK" },
            { 301, "Moved Permanently" },
            { 400, "Bad Request" },
            { 401, "Not Authorized" },
            { 403, "Forbidden" },
            { 404, "Not Found" },
            { 500, "Internal Server Error" }
        };

        Response::Response()
        {
        }


        Response::~Response()
        {
        }

    } //namespace Http
} //namespace Naive
