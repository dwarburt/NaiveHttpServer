
#include "HttpServer.hpp"
#include <boost/asio.hpp>
#include "HttpUtil.hpp"
#include "HttpUrl.hpp"

using namespace Naive::Http;

Response handle(Request req)
{
    std::string prolog = "<html><body>";
    std::string title = "<h1>header count: " + std::to_string(req.get_headers().size()) + "</h1>";
    std::string request_echo = "<pre>" + req.to_string() + "</pre>";
    std::string seperator = "<hr>";
    auto url = req.get_url();
    std::string query_details = "<table><tr><td>protocol</td><td>" + url.get_protocol() + "</td></tr>" +
        "<tr><td>host</td><td>" + url.get_host() + "</td></tr>" +
        "<tr><td>path</td><td>" + url.get_path() + "</td></tr>";

    for (auto p : url.get_query())
    {
        query_details += "<tr><td>" + p.first + "</td><td>" + p.second + "</td></tr>";
    }
    query_details += "</table>";
    std::string epilog = "</body></html>";

    auto j = Response(
        prolog +
        title +
        request_echo +
        seperator +
        query_details +
        epilog
        );
    j.set_header("Content-Type", "text/html");

    return j;
}
void test(std::string x)
{
    std::cout << "\"" << Url::percent_decode(x) << "\"" << std::endl;
}
int main(int argc, char **argv)
{
    Server my_server;
    my_server.route_to_files("/static", ".");
    my_server.start(handle);
    return 0;
}

