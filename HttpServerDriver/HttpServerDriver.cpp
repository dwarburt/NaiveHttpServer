
#include "stdafx.h"
#include "HttpServer.h"
#include <boost/asio.hpp>
int main(int argc, char **argv)
{
    HttpServer my_server;
    my_server.start();
    return 0;
}

