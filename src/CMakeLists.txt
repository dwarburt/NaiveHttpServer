ExternalProject_Add(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-11-0
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Get_Property(asio source_dir)
set(headers ${CMAKE_SOURCE_DIR}/include ${source_dir}/asio/include)
set(libs "")
find_package(OpenSSL)
if (${OPENSSL_FOUND})
    set(libs ${libs} ${OPENSSL_CRYPTO_LIBRARY})
    set(headers ${headers} ${OPENSSL_INCLUDE_DIR})
else(${OPENSSL_FOUND})
    MESSAGE(FATAL_ERROR "Could not find OpenSSL")
endif(${OPENSSL_FOUND})

find_package(CURL)
if (${CURL_FOUND})
    set(libs ${libs} ${CURL_LIBRARIES})
    set(headers ${headers} ${CURL_INCLUDE_DIRECTORIES})
else(${CURL_FOUND})
    MESSAGE(FATAL_ERROR "Could not find CURL")
endif(${CURL_FOUND})



include_directories(${headers})
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

add_dependencies(ihttp asio)
target_link_libraries(ihttp ${libs})
target_include_directories(ihttp PUBLIC ${headers})
