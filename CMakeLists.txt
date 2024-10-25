cmake_minimum_required(VERSION 3.10)

project(C_custom_allocator)

set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

include_directories(include)

set(SRC
    src/main.c
    src/n_allocator.c
    src/mem_chunk.c
    src/mem_chunk_list.c
    src/heap.c
    src/n_allocator_internal.c
    src/n_reallocator.c
    src/n_allocator_printer.c
)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra)
endif()

add_executable(exe ${SRC})
