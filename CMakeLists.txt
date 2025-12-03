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

function(cmpi_add_server_test NAME)
    add_executable(c-mpi_${NAME} "test/${NAME}.cpp")
    target_link_libraries(c-mpi_${NAME} PRIVATE c-mpi)
    add_test(NAME c-mpi_${NAME} COMMAND c-mpi_${NAME})
    set_tests_properties(c-mpi_${NAME} PROPERTIES
        SKIP_REGULAR ON 
        LABELS "ServerRequired" 
    )
endfunction()

cmpi_add_server_test(setblock)
cmpi_add_server_test(blockmodifying)
cmpi_add_server_test(placesign)
