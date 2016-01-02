
#include "HttpServer.hpp"
#include <boost/asio.hpp>

using namespace Naive::Http;

Response handle(Request req)
{
    return Response("header count: " + std::to_string(req.get_headers().size()) + req.to_string());
}
int main(int argc, char **argv)
{
    Server my_server;
    my_server.start(handle);
    return 0;
}

