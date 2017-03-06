ExternalProject_Add(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-11-0
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Get_Property(asio source_dir)
set(headers ${headers} ${CMAKE_SOURCE_DIR}/include ${source_dir}/asio/include)
set(deps ${deps} asio)
