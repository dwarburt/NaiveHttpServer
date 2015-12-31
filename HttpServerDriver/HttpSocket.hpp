#pragma once
#include <functional>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

namespace Naive
{
    namespace Http
    {
        typedef std::function<Response(Request)> RequestHandler;

        class Socket
        {
        public:
            Socket();
            ~Socket();
        };
    }
}

