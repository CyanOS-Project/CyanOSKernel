include_directories(".")

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -O0 -gdwarf")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0 -gdwarf")
SET(CMAKE_EXE_LINKER_FLAGS  "--target=i686 -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -nostdlib")

file(GLOB_RECURSE SOURCES "*.cpp" "*.asm")
add_executable(${OUTPUT_FILE} ${SOURCES})
add_custom_command(
    TARGET ${OUTPUT_FILE}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary $<TARGET_FILE:${OUTPUT_FILE}> ${OUTPUT_IMAGE}
    COMMENT "Striping the kernel output file..."
)
