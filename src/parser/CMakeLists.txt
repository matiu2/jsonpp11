project (parser)

add_executable(test_number test_number.cpp)
target_link_libraries(test_number ${CPP})
add_test(test_number test_number)

add_executable(test_outer test_outer.cpp)
target_link_libraries(test_outer ${CPP})
add_test(test_outer test_outer)

add_executable(test_string test_string.cpp)
target_link_libraries(test_string ${CPP})
add_test(test_string test_string)

add_executable(test_object test_object.cpp)
target_link_libraries(test_object ${CPP})
add_test(test_object test_object)

add_executable(test_array test_array.cpp)
target_link_libraries(test_array ${CPP})
add_test(test_array test_array)

install(FILES error.hpp number.hpp outer.hpp status.hpp string.hpp object.hpp array.hpp utf8_writer.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include/jsonpp11/parser)
