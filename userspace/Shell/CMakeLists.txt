include_directories("." "../../")

SET(OUTPUT_FILE "Shell")

file(GLOB SOURCES CONFIGURE_DEPENDS "*.c" "*.cpp" "*.asm")
add_executable(${OUTPUT_FILE} ${SOURCES})
target_link_libraries(${OUTPUT_FILE} "libsystem")

add_custom_command(
    TARGET ${OUTPUT_FILE}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:${OUTPUT_FILE}> ${USER_DIRECTORY}
    COMMENT "copying to UserBinary directory ..."
)

