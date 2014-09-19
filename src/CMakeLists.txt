project(src)

add_executable(test_unicode test_unicode.cpp)
add_test(test_unicode test_unicode)

add_subdirectory(parser)
