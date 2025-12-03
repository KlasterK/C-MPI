#include <iostream>
#include "minecraft.h"
#include "commontest.hpp"

int main()
{
    cmpi_connection_t conn;
    int id, state;

    _TST(cmpi_net_connect_default, &conn);
    _TST(
        cmpi_set_standing_sign, &conn, DFLX + 2, DFLY, DFLZ, CMPI_STANDING_SIGN_SSE, 
        "Hello", "", "There", "World"
    );
    _TST(cmpi_set_block, &conn, DFLX + 3, DFLY, DFLZ + 1, STONE);
    _TST(
        cmpi_set_wall_sign, &conn, DFLX + 3, DFLY, DFLZ, CMPI_WALL_SIGN_NORTH,
        "What", "Isssssssssssssssssssssssssssssssssss", "End", "Llllllllllllllllllooooooonnnnnggggg"
    );

    _TST(cmpi_net_close, &conn);

    return 0;
}
