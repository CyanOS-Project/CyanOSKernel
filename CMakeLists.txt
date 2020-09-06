cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_ASM_NASM_OBJECT_FORMAT win32)
enable_language(ASM_NASM)
project(CyanOS VERSION 0.1.0)


SET(OUTPUT_FILE "kernel")
SET(OUTPUT_IMAGE ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILE}.img)
SET(BINS_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/apps)
SET(DRVS_DIRECTORY ${BINS_DIRECTORY}/Drivers)
SET(USER_DIRECTORY ${BINS_DIRECTORY}/UserBinary)
SET(ROOT_DIRECTORY "CyanOS_root")
SET(OUTPUT_ISO "CyanOS.iso")
SET(QEMU qemu-system-i386)
SET(QEMU_FLAGS -serial stdio  -serial file:CON -no-reboot -no-shutdown -m 128 -d cpu_reset -boot d -cdrom)
SET(QEMU_FLAGS_DEBUG -serial stdio  -serial file:CON -S -gdb tcp::99999)
file(MAKE_DIRECTORY ${BINS_DIRECTORY} ${DRVS_DIRECTORY} ${USER_DIRECTORY})

#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")

add_subdirectory("kernel")


add_custom_command(
        OUTPUT  ${OUTPUT_ISO}
        MAIN_DEPENDENCY  ${OUTPUT_FILE}
        COMMAND python ../utils/make_bootable_iso.py ${OUTPUT_IMAGE} ${BINS_DIRECTORY} ${ROOT_DIRECTORY} ${OUTPUT_ISO}
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

enable_testing()
add_subdirectory("googletest")
add_subdirectory("test")
include(CTest)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
