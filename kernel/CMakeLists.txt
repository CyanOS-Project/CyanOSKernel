include_directories(".")

#set(CMAKE_CXX_STANDARD 17)
SET(GCC_COVERAGE_COMPILE_FLAGS "-m32 -masm=intel -ggdb -Wall -Wextra --std=c++17 -Og -MMD -fno-pie -ffreestanding -fno-rtti -fno-exceptions -fno-leading-underscore -Werror=return-type")
SET(GCC_COVERAGE_LINK_FLAGS    "-T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -m32 -nostdlib")
SET(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS})
SET(CMAKE_EXE_LINKER_FLAGS  ${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS})

file(GLOB_RECURSE SOURCES "*.cpp" "*.asm")
add_executable(${OUTPUT_FILE} ${SOURCES})
add_custom_command(
    TARGET ${OUTPUT_FILE}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary $<TARGET_FILE_NAME:${OUTPUT_FILE}> ${OUTPUT_IMAGE}
    COMMENT "Striping the kernel output file..."
)
