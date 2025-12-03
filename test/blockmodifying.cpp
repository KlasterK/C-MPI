#include "minecraft.h"
#include <iostream>
#include <chrono>
#include <thread>
#include "commontest.hpp"

using namespace std::chrono_literals;
constexpr int X = DFLX + 1;

int main()
{
    cmpi_connection_t conn;
    int block_id, new_block_id;

    _TST(cmpi_net_connect_default, &conn);
    _STORE(block_id, cmpi_get_block, &conn, X, DFLY, DFLZ);
    if(block_id != GOLD_BLOCK)
        block_id = GOLD_BLOCK;
    else
        block_id = IRON_BLOCK;

    _TST(cmpi_set_block, &conn, X, DFLY, DFLZ, block_id);
    std::this_thread::sleep_for(100ms);
    _STORE(new_block_id, cmpi_get_block, &conn, X, DFLY, DFLZ);
    // TODO: Assertion is intermittently failing
    // Although in game, block is set correctly
    // The assertion fails non-deterministically, succeeding most of the time
    _ASSERT(new_block_id == block_id); 

    _TST(cmpi_net_close, &conn);

    return 0;
}
