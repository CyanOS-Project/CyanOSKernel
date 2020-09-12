include_directories(".")
include_directories("../libraries/stdlib")

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -O0 -gdwarf")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0 -gdwarf")
SET(CMAKE_EXE_LINKER_FLAGS  "--target=i686 -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -nostdlib")

file(GLOB_RECURSE SOURCES "*.cpp" "*.asm" "../stdlib/*.cpp")
add_executable(${KERNEL_EXE} ${SOURCES})
target_link_libraries(${KERNEL_EXE} "stdlib")
add_custom_command(
    TARGET ${KERNEL_EXE}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary $<TARGET_FILE:${KERNEL_EXE}> ${KERNEL_IMAGE}
    COMMENT "Striping the kernel output file..."
)