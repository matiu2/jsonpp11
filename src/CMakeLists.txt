project(src)

add_executable(test_unicode test_unicode.cpp)
target_link_libraries(test_unicode ${CPP})
add_test(test_unicode test_unicode)

add_executable(test_json_class test_json_class.cpp)
target_link_libraries(test_json_class ${CPP})
add_test(test_json_class test_json_class)

add_executable(test_parse_to_json_class test_parse_to_json_class.cpp)
target_link_libraries(test_parse_to_json_class ${CPP})
add_test(test_parse_to_json_class test_parse_to_json_class)

file(COPY sample.json DESTINATION .)

add_subdirectory(parser)

install(FILES unicode.hpp utils.hpp json_class.hpp parser.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include/jsonpp11)
