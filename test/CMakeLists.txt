cmake_minimum_required(VERSION 3.0.0)
include_directories("../libraries/stdlib")
enable_testing()

add_compile_definitions("__UNIT_TESTS")
SET(This "tests")
file(GLOB_RECURSE SOURCES_TESTS CONFIGURE_DEPENDS "*.cpp")
file(GLOB SOURCES CONFIGURE_DEPENDS "../kernel/Utils/String.cpp"
                                    "../kernel/Utils/StringView.cpp"
                                    "../kernel/Utils/Bitmap.cpp"
                                    "../kernel/Utils/PathParser.cpp")
add_executable(${This} ${SOURCES} ${SOURCES_TESTS})
target_link_libraries(${This} PUBLIC "gtest_main" "stdlib")
add_test(
    NAME ${This}
    COMMAND ${This}
)