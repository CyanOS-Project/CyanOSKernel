cmake_minimum_required(VERSION 3.0.0)
include_directories("../libraries")
enable_testing()



add_compile_definitions("__UNIT_TESTS")
SET(This "tests")
file(GLOB SOURCES_TESTS CONFIGURE_DEPENDS "./*.cpp")
file(GLOB SOURCES CONFIGURE_DEPENDS "../libraries/stdlib/String.cpp"
                                    "../libraries/stdlib/StringView.cpp"
                                    "../libraries/stdlib/Bitmap.cpp"
                                    "../libraries/stdlib/PathParser.cpp")

add_executable(${This} ${SOURCES} ${SOURCES_TESTS})
target_link_libraries(${This} PUBLIC "gtest_main")
add_test(
    NAME ${This}
    COMMAND ${This}
)