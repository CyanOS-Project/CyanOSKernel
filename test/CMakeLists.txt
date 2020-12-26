cmake_minimum_required(VERSION 3.0.0)
include_directories("../libraries")
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)

SET(CMAKE_C_FLAGS  ${CMAKE_C_FLAGS} "-fsanitize=address,undefined")
SET(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} "-fsanitize=address,undefined")
SET(CMAKE_EXE_LINKER_FLAGS  ${CMAKE_EXE_LINKER_FLAGS} "-fsanitize=address,undefined")

add_compile_definitions("__UNIT_TESTS")
SET(This "tests")
file(GLOB SOURCES_TESTS CONFIGURE_DEPENDS "./*_Test.cpp")
file(GLOB SOURCES CONFIGURE_DEPENDS "../libraries/stdlib/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "Clib.cpp")

add_executable(${This} ${SOURCES} ${SOURCES_TESTS})
target_link_libraries(${This} PUBLIC "gtest_main")
add_test(
    NAME ${This}
    COMMAND ${This}
)