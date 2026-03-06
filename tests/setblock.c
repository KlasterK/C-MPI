#include "minecraft.h"
#include "commontest.h"

int main()
{
    cmpi_connection_t conn;

    _TST(cmpi_net_connect_default, &conn);
    _TST(cmpi_set_block, &conn, DFLX, DFLY, DFLZ, STONE);
    _TST(cmpi_net_close, &conn);

    return 0;
}
