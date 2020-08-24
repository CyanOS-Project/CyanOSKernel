cmake_minimum_required(VERSION 3.0.0)
#set(CMAKE_CXX_STANDARD 17)
include_directories("../kernel")
enable_testing()

add_compile_definitions("__UNIT_TESTS")
SET(This "tests")
file(GLOB_RECURSE SOURCES_TESTS "*.cpp")
file(GLOB SOURCES   "../kernel/Utils/String.cpp"
                    "../kernel/Utils/StringView.cpp"
                    "../kernel/Utils/bitmap.cpp"
                    "../kernel/Utils/PathParser.cpp")
add_executable(${This} ${SOURCES} ${SOURCES_TESTS})
target_link_libraries(${This} PUBLIC gtest_main)
add_test(
    NAME ${This}
    COMMAND ${This}
)