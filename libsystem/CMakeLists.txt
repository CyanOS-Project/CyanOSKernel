include_directories(".")

SET(OUTPUT_FILE "libsystem")

file(GLOB SOURCES CONFIGURE_DEPENDS "Systemcalls/*.cpp")
add_library(${OUTPUT_FILE} ${SOURCES})
