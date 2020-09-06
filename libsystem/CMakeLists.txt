include_directories(".")

#set(CMAKE_CXX_STANDARD 17)
SET(OUTPUT_FILE "libsystem")
SET(GCC_COVERAGE_COMPILE_FLAGS "-m32 -masm=intel -Wall -Wextra --std=c++17 -Og -MMD -fno-pie -ffreestanding -fno-rtti -fno-exceptions -fno-leading-underscore -Werror=return-type")
SET(GCC_COVERAGE_LINK_FLAGS    "-m32 -nostdlib")
SET(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS})
SET(CMAKE_EXE_LINKER_FLAGS  ${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS})


file(GLOB_RECURSE SOURCES "*.c" "*.cpp" "*.asm")
add_library(${OUTPUT_FILE} ${SOURCES})
