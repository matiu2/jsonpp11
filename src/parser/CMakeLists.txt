project (parser)

add_executable(test_number test_number.cpp)
add_test(test_number test_number)

add_executable(test_outer test_outer.cpp)
add_test(test_outer test_outer)

add_executable(test_string test_string.cpp)
add_test(test_string test_string)

add_executable(test_object test_object.cpp)
add_test(test_object test_object)

install(FILES error.hpp number.hpp outer.hpp status.hpp string.hpp object.hpp utf8_writer.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include/jsonpp11/parser)
