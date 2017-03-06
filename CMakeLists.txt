cmake_minimum_required(VERSION 2.8.11)
include(ExternalProject)

project(ihttp)

set(defines
    -w
    -fpermissive
    -Wno-unused-variable
    -std=c++11
    -fvisibility=hidden
    -D_WEBSOCKETPP_CPP11_STL_
    -DASIO_STANDALONE
)

add_definitions(${defines})

add_subdirectory(src)
add_subdirectory(test)
