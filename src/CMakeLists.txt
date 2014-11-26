project(src)

add_executable(test_unicode test_unicode.cpp)
add_test(test_unicode test_unicode)

add_executable(test_json_class test_json_class.cpp)
add_test(test_json_class test_json_class)

add_subdirectory(parser)

install(FILES unicode.hpp utils.hpp json_class.hpp parser.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include/jsonpp11)
