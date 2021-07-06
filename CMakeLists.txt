cmake_minimum_required(VERSION 2.4)
set(PROJECT_SOURCE_DIR ".")
set(PROJECT_BINARY_DIR ".")
project(simulator)

set(CMAKE_C_COMPILER "/usr/bin/cc")
set(COMMON_FLAGS "-Wall -Wextra -Wconversion -Wshadow -Wno-unused-parameter -std=gnu11")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g -DDEBUG=1 ${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "-O3 -s ${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -g ${COMMON_FLAGS}")

add_executable(simulator main.c gl.c core.c os.c mmath.c)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(GL REQUIRED gl)

target_link_libraries(simulator ${SDL2_LIBRARIES} ${GL_LIBRARIES} m)
target_include_directories(simulator PUBLIC ${SDL2_INCLUDE_DIRS})
target_compile_options(simulator PUBLIC ${SDL2_CFLAGS_OTHER} ${GL_CFLAGS_OTHER})

