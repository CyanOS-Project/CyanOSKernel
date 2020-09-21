include_directories(".")
include_directories("../libraries/stdlib")


SET(triple i686-none-windows)
SET(CMAKE_C_COMPILER_TARGET ${triple})
SET(CMAKE_CXX_COMPILER_TARGET ${triple})
SET(CMAKE_C_FLAGS  "--target=${triple} -march=i686 -O0 -gdwarf -Wall -Wextra --pedantic -ffreestanding")
SET(CMAKE_CXX_FLAGS  "--target=${triple} -march=i686 -O0 -gdwarf -Wall -Wextra --pedantic -ffreestanding -fno-exceptions -fno-rtti")
SET(CMAKE_EXE_LINKER_FLAGS  "--target=i686 -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -nostdlib")



add_compile_definitions("CYANOS_KENREL")

file(GLOB_RECURSE SOURCES "*.cpp" "*.asm" "../stdlib/*.cpp")
add_executable(${KERNEL_EXE} ${SOURCES})
target_link_libraries(${KERNEL_EXE} "stdlib")
add_custom_command(
    TARGET ${KERNEL_EXE}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary $<TARGET_FILE:${KERNEL_EXE}> ${KERNEL_IMAGE}
    COMMENT "Striping the kernel output file..."
)