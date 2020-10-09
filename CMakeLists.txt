SET(CMAKE_TOOLCHAIN_FILE "Toolchain.i686-pe.cmake")

cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_ASM_NASM_OBJECT_FORMAT win32)
enable_language(ASM_NASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(CyanOS VERSION 0.1.0)

SET(KERNEL_EXE "kernel.bin")
SET(KERNEL_IMAGE "${CMAKE_CURRENT_BINARY_DIR}/kernel.img")
SET(BINS_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
SET(DRIVERS_BIN_DIRECTORY "${BINS_DIRECTORY}/drivers")
SET(USER_BIN_DIRECTORY "${BINS_DIRECTORY}/bin")
SET(ROOT_DIRECTORY "CyanOS_root")
SET(OUTPUT_ISO "CyanOS.iso")
SET(QEMU qemu-system-i386)
SET(QEMU_FLAGS -accel tcg,thread=single -cpu core2duo -m 128 -smp 1 -serial stdio  -serial file:CON -no-reboot -no-shutdown -d cpu_reset -boot d -cdrom)
SET(QEMU_FLAGS_DEBUG -S -gdb tcp::99999)
file(MAKE_DIRECTORY ${BINS_DIRECTORY} ${DRIVERS_BIN_DIRECTORY} ${USER_BIN_DIRECTORY})
set_property(GLOBAL PROPERTY generated_bins)


function(add_generated_bin element)
    get_property(tmp GLOBAL PROPERTY generated_bins)
    set(tmp "${element};${tmp}")
    set_property(GLOBAL PROPERTY generated_bins "${tmp}")
endfunction(add_generated_bin)

macro(generate_user_bin bin_name bin_libs)
    include_directories("../../libraries" "${PROJECT_SOURCE_DIR}" ".")
    file(GLOB SOURCES CONFIGURE_DEPENDS "*.c" "*.cpp" "*.asm")
    add_executable(${bin_name} ${SOURCES})
    target_link_libraries(${bin_name} ${bin_libs} "stdlib")
    add_custom_command(
        TARGET ${bin_name}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:${bin_name}> ${USER_BIN_DIRECTORY}
    )
    add_generated_bin(${bin_name})
endmacro(generate_user_bin)

macro(generate_library lib_name)
    include_directories("${PROJECT_SOURCE_DIR}" ".")
    file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp")
    add_library(${lib_name} ${SOURCES})
endmacro(generate_library)

add_subdirectory("kernel")
add_subdirectory("userspace")
add_subdirectory("libraries")


get_property(__generated_bins GLOBAL PROPERTY generated_bins)
add_custom_command(
        OUTPUT  ${OUTPUT_ISO}
        DEPENDS  ${KERNEL_EXE} ${__generated_bins}
        COMMAND python ../build_tools/make_bootable_iso.py ${KERNEL_IMAGE} ${BINS_DIRECTORY} ${ROOT_DIRECTORY} ${OUTPUT_ISO}
        COMMENT "Creating ISO file..."
)

add_custom_target(compile ALL DEPENDS ${OUTPUT_ISO})

add_custom_target(debug
    DEPENDS ${OUTPUT_ISO}
    COMMAND ${QEMU} ${QEMU_FLAGS} ${OUTPUT_ISO} ${QEMU_FLAGS_DEBUG}
)

add_custom_target(run
    DEPENDS ${OUTPUT_ISO}
    COMMAND ${QEMU} ${QEMU_FLAGS} ${OUTPUT_ISO}
)

#------------------------------------- Testing-------------------------------------------------

enable_testing()
#add_subdirectory("googletest")
#add_subdirectory("test")
include(CTest)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)