cmake_minimum_required(VERSION 3.0.0)
include_directories("../libraries")


add_compile_definitions("__UNIT_TESTS")
SET(This "tests")
file(GLOB SOURCES_TESTS CONFIGURE_DEPENDS "./*_Test.cpp")
file(GLOB SOURCES CONFIGURE_DEPENDS "../libraries/stdlib/String.cpp"
                                    "../libraries/stdlib/PathView.cpp"
                                    "../libraries/stdlib/ArgumentParser.cpp"
                                    "../libraries/stdlib/StringView.cpp"
                                    "../libraries/stdlib/Math.cpp"
                                    "../libraries/stdlib/Clib.cpp"
                                    )
list(FILTER SOURCES EXCLUDE REGEX "Clib.cpp")

add_executable(${This} ${SOURCES} ${SOURCES_TESTS})
target_link_libraries(${This} PUBLIC "gtest_main")
add_test(
    NAME ${This}
    COMMAND ${This}
)