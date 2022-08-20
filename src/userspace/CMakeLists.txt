SET(triple i686-none-windows)
SET(CMAKE_C_COMPILER_TARGET ${triple})
SET(CMAKE_CXX_COMPILER_TARGET ${triple})


SET(CMAKE_C_FLAGS  "-Wall -Wextra --pedantic -ffreestanding")
SET(CMAKE_CXX_FLAGS  "-Wall -Wextra --pedantic -ffreestanding -fno-exceptions -fno-rtti")
SET(CMAKE_EXE_LINKER_FLAGS  "-nostdlib -Wl,-estartup")

include_directories("../libraries/stdlib")
add_subdirectory("Shell")
add_subdirectory("TestApp")
add_subdirectory("Cat")

