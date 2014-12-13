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

file(COPY sample.json sample2.json DESTINATION .)

add_subdirectory(parser)

install(FILES json_class.hpp parse_to_json_class.hpp unicode.hpp utils.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include/jsonpp11)
