
#include "HttpServer.hpp"
#include <boost/asio.hpp>

using namespace Naive::Http;

Response handle(Request req)
{
    auto j = Response("<html><body><h1>header count: " + std::to_string(req.get_headers().size()) +"</h1><pre>" +  req.to_string() + "</pre></body></html>");
    j.set_header("Content-Type", "text/html");

    return j;
}
int main(int argc, char **argv)
{
    Server my_server;
    my_server.start(handle);
    return 0;
}

