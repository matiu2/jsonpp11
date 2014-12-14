project (parser)

if (${BUILD_TESTS})
    add_executable(test_number test_number.cpp)
    add_dependencies(test_number bandit)
    target_link_libraries(test_number ${CPP})
    add_test(test_number test_number)

    add_executable(test_outer test_outer.cpp)
    add_dependencies(test_outer bandit)
    target_link_libraries(test_outer ${CPP})
    add_test(test_outer test_outer)

    add_executable(test_string test_string.cpp)
    add_dependencies(test_string bandit)
    target_link_libraries(test_string ${CPP})
    add_test(test_string test_string)

    add_executable(test_object test_object.cpp)
    add_dependencies(test_object bandit)
    target_link_libraries(test_object ${CPP})
    add_test(test_object test_object)

    add_executable(test_array test_array.cpp)
    add_dependencies(test_array bandit)
    target_link_libraries(test_array ${CPP})
    add_test(test_array test_array)
endif()

install(FILES LocatingIterator.hpp array.hpp error.hpp number.hpp object.hpp outer.hpp status.hpp string.hpp utf8_writer.hpp utils.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include/jsonpp11/parser)
