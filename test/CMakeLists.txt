SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS ON)

SET(STDLIB_PATH "../src/libraries/stdlib")

project(CyanOSTests)

cmake_minimum_required(VERSION 3.0.0)
include_directories(${STDLIB_PATH})
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)

SET(CMAKE_C_FLAGS  ${CMAKE_C_FLAGS} "-fsanitize=address,undefined")
SET(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} "-fsanitize=address,undefined")
SET(CMAKE_EXE_LINKER_FLAGS  ${CMAKE_EXE_LINKER_FLAGS} "-fsanitize=address,undefined")

if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    enable_testing()
    add_subdirectory("googletest")
    include(CTest)
    set(CPACK_PROJECT_NAME ${PROJECT_NAME})
    set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
    include(CPack)
endif()

add_compile_definitions("__UNIT_TESTS")
SET(This "tests")
file(GLOB SOURCES_TESTS CONFIGURE_DEPENDS "./*_Test.cpp")
file(GLOB SOURCES CONFIGURE_DEPENDS "${STDLIB_PATH}/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "Clib.cpp")

add_executable(${This} ${SOURCES} ${SOURCES_TESTS})
target_link_libraries(${This} PUBLIC "gtest_main")
add_test(
    NAME ${This}
    COMMAND ${This}
)