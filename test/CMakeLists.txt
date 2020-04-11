include (../cmake/gtest.cmake)
set(libs ${libs} ihttp pthread)
add_executable(ihttp_test http_test.cpp)
target_link_libraries(ihttp_test LINK_PUBLIC ${libs})
