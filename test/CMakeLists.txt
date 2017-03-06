include(ExternalProject)

ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    CMAKE_ARGS ${TOOLCHAIN_CMAKE_ARG}
    LOG_BUILD 1
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(googletest source_dir)
ExternalProject_Get_Property(googletest binary_dir)

include_directories(
    ${source_dir}/googletest/include
    ${source_dir}/googlemock/include
)
add_library(gtest UNKNOWN IMPORTED)
add_library(gmock UNKNOWN IMPORTED)

set_property(TARGET gtest PROPERTY IMPORTED_LOCATION ${GTEST_BINARY_DIR}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET gmock PROPERTY IMPORTED_LOCATION ${GTEST_BINARY_DIR}/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}gmock${CMAKE_STATIC_LIBRARY_SUFFIX})

add_executable(ihttp_test http_test.cpp)
target_link_libraries(ihttp_test LINK_PUBLIC ihttp gtest gmock)
