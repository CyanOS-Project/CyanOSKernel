include_directories(".")

SET(OUTPUT_FILE "libsystem")
SET(CMAKE_EXE_LINKER_FLAGS  "--target=i686-unknown-windows -nostdlib -Wl,-entry:main  -Wl,-subsystem:native -fuse-ld=lld-link")


file(GLOB SOURCES CONFIGURE_DEPENDS "Systemcalls/*.cpp")
add_library(${OUTPUT_FILE} ${SOURCES})
