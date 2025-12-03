cmake_minimum_required(VERSION 3.10)
project(C-MPI LANGUAGES C CXX VERSION 0.1)

find_path(LIBUV_INCLUDE_DIR NAMES uv.h)
find_library(LIBUV_LIBRARIES NAMES uv libuv)

add_library(c-mpi STATIC
    src/minecraft.c
    src/net.c
)
target_include_directories(c-mpi PUBLIC include)
target_include_directories(c-mpi PUBLIC ${LIBUV_INCLUDE_DIR})
target_link_libraries(c-mpi PUBLIC ${LIBUV_LIBRARIES})

install(DIRECTORY include/ DESTINATION include/c-mpi)
install(TARGETS c-mpi DESTINATION lib)

#####################

enable_testing()

add_executable(c-mpi_setblock test/setblock.cpp)
target_link_libraries(c-mpi_setblock PRIVATE c-mpi)
add_test(NAME c-mpi_setblock COMMAND c-mpi_setblock)
set_tests_properties(c-mpi_setblock PROPERTIES
    SKIP_REGULAR ON
    LABELS "ServerRequired"
)

add_executable(c-mpi_blockmodifying test/blockmodifying.cpp)
target_link_libraries(c-mpi_blockmodifying PRIVATE c-mpi)
add_test(NAME c-mpi_blockmodifying COMMAND c-mpi_blockmodifying)
set_tests_properties(c-mpi_blockmodifying PROPERTIES
    SKIP_REGULAR ON
    LABELS "ServerRequired"
)
