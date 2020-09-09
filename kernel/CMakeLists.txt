include_directories(".")

SET(CMAKE_EXE_LINKER_FLAGS  "--target=i686 -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -nostdlib")
message("--------------------------Flags--------------------------------")
message(${CMAKE_LINKER})
message(${CMAKE_EXE_LINKER_FLAGS})
message(${CMAKE_EXE_LINKER_FLAGS_INIT})
message("--------------------------Flags--------------------------------")

file(GLOB_RECURSE SOURCES "*.cpp" "*.asm")
add_executable(${OUTPUT_FILE} ${SOURCES})
add_custom_command(
    TARGET ${OUTPUT_FILE}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary $<TARGET_FILE_NAME:${OUTPUT_FILE}> ${OUTPUT_IMAGE}
    COMMENT "Striping the kernel output file..."
)
