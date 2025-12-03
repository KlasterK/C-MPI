#include "minecraft.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

#define _TST(fn, conn, ...) \
{ \
    (fn)((conn), ##__VA_ARGS__); \
    if((conn)->error_state) \
    { \
        std::cout << #fn \
                  << ": is_connected=" << (conn)->is_connected \
                  << ", error_state=" << (conn)->error_state << std::endl; \
        exit((conn)->error_state); \
    } \
}

#define _STORE(dst, fn, conn, ...) \
{ \
    (dst) = (fn)((conn), ##__VA_ARGS__); \
    if((conn)->error_state) \
    { \
        std::cout << #fn \
                  << ": is_connected=" << (conn)->is_connected \
                  << ", error_state=" << (conn)->error_state << std::endl; \
        exit((conn)->error_state); \
    } \
}

#define _ASSERT(expr) \
{ \
    if(!!(expr)) \
    { \
        std::cout << "Assertion failed: " << #expr << std::endl; \
        exit(1); \
    } \
}

constexpr int GOLD_BLOCK = 41;
constexpr int IRON_BLOCK = 42;

#define coords 2, 66, -83

int main()
{
    cmpi_connection_t conn;
    int block_id, new_block_id;

    _TST(cmpi_net_connect_default, &conn);
    _STORE(block_id, cmpi_get_block, &conn, coords);
    if(block_id != GOLD_BLOCK)
        block_id = GOLD_BLOCK;
    else
        block_id = IRON_BLOCK;

    _TST(cmpi_set_block, &conn, coords, block_id);
    std::this_thread::sleep_for(100ms);
    _STORE(new_block_id, cmpi_get_block, &conn, coords);
    // TODO: Assertion is intermittently failing
    // Although in game, block is set correctly
    // The assertion fails non-deterministically, succeeding most of the time
    _ASSERT(new_block_id == block_id); 

    _TST(cmpi_net_close, &conn);

    return 0;
}
