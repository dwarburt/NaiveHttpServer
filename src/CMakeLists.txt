set(headers "")
set(libs "")
set(deps "")
include(../cmake/asio.cmake)
include(../cmake/openssl.cmake)
include(../cmake/curl.cmake)

add_library(ihttp
    auth.cpp
    client.cpp
    message.cpp
    request.cpp
    response.cpp
    server.cpp
    socket.cpp
    url.cpp
    util.cpp)

include_directories(${headers})
add_dependencies(ihttp ${deps})
target_link_libraries(ihttp ${libs})
target_include_directories(ihttp PUBLIC ${headers})
