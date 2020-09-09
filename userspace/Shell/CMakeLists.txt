include_directories("." "../../")

#set(CMAKE_CXX_STANDARD 17)
SET(OUTPUT_FILE "Shell")
SET(CMAKE_EXE_LINKER_FLAGS  "--target=i686-unknown-windows -nostdlib -Wl,-entry:main -fuse-ld=lld-link")

message("--------------------------Flags Shell--------------------------------")
message(${CMAKE_CXX_COMPILER})
message(${CMAKE_CXX_FLAGS})
message(${CMAKE_LINKER})
message(${CMAKE_EXE_LINKER_FLAGS})
message(${CMAKE_EXE_LINKER_FLAGS_INIT})
message("--------------------------Flags--------------------------------")

file(GLOB SOURCES CONFIGURE_DEPENDS "*.c" "*.cpp" "*.asm")
add_executable(${OUTPUT_FILE} ${SOURCES})
#target_link_libraries(${OUTPUT_FILE} "libsystem")

add_custom_command(
    TARGET ${OUTPUT_FILE}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:${OUTPUT_FILE}> ${USER_DIRECTORY}
    COMMENT "copying to UserBinary directory ..."
)

