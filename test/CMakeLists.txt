ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    CMAKE_ARGS ${TOOLCHAIN_CMAKE_ARG}
    LOG_BUILD 1
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(googletest source_dir)
ExternalProject_Get_Property(googletest binary_dir)

add_library(gtest UNKNOWN IMPORTED)
add_library(gmock UNKNOWN IMPORTED)

set_property(TARGET gtest PROPERTY IMPORTED_LOCATION ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET gmock PROPERTY IMPORTED_LOCATION ${binary_dir}/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}gmock${CMAKE_STATIC_LIBRARY_SUFFIX})

include_directories(
    ${source_dir}/googletest/include
    ${source_dir}/googlemock/include
)

set(libs ihttp gtest gmock pthread)



add_executable(ihttp_test http_test.cpp)

add_dependencies(ihttp_test googletest)
target_link_libraries(ihttp_test LINK_PUBLIC ${libs})
