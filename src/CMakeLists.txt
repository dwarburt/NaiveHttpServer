set(headers ${CMAKE_SOURCE_DIR}/include)

include_directories(${headers})
add_library(ihttp
    auth.cpp
    message.cpp
    request.cpp
    response.cpp
    server.cpp
    socket.cpp
    url.cpp
    util.cpp)

target_include_directories(ihttp PUBLIC ${headers})
