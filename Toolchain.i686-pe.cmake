SET(CMAKE_SYSTEM_NAME "Generic")
SET(CMAKE_SYSTEM_VERSION 1)

SET(triple i686-none-windows)
SET(CMAKE_C_COMPILER_ID "Clang")
SET(CMAKE_CXX_COMPILER_ID "Clang")
SET(CMAKE_C_COMPILER_TARGET ${triple})
SET(CMAKE_CXX_COMPILER_TARGET ${triple})


SET(CMAKE_C_FLAGS  "--target=${triple} -march=i686 -Wall -Wextra --pedantic -ffreestanding")
SET(CMAKE_CXX_FLAGS  "--target=${triple} -march=i686 -Wall -Wextra --pedantic -ffreestanding -fno-exceptions -fno-rtti")
SET(CMAKE_EXE_LINKER_FLAGS_INIT  "--target=${triple} -march=i686 -nostdlib -Wl,-entry:main")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)