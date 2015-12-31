
#include "stdafx.h"
#include "HttpServer.hpp"
#include <boost/asio.hpp>

HttpResponse handle(HttpRequest req)
{
    return HttpResponse();
}
int main(int argc, char **argv)
{
    HttpServer my_server;
    my_server.start(handle);
    return 0;
}

