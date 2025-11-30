cmake_minimum_required(VERSION 3.10)
project(C-MPI LANGUAGES C VERSION 0.1)

find_path(LIBUV_INCLUDE_DIR NAMES uv.h)
find_library(LIBUV_LIBRARIES NAMES uv libuv)

add_library(c-mpi STATIC
    src/minecraft.c
    src/net.c
)
target_include_directories(c-mpi PUBLIC include)
target_include_directories(c-mpi PUBLIC ${LIBUV_INCLUDE_DIR})
target_link_libraries(c-mpi PUBLIC ${LIBUV_LIBRARIES})

install(DIRECTORY include DESTINATION include/c-mpi)
install(TARGETS c-mpi DESTINATION lib)
