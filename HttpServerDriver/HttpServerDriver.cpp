
#include "HttpServer.hpp"
#include <boost/asio.hpp>

using namespace Naive::Http;

Response handle(Request req)
{
    return Response("Well OK then.");
}
int main(int argc, char **argv)
{
    Server my_server;
    my_server.start(handle);
    return 0;
}

