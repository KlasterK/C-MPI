#include "minecraft.h"

int cmpi_get_block(cmpi_connection_t *conn, int x, int y, int z)
{
    cmpi_net_printf(conn, "world.getBlock(%d,%d,%d)\n", x, y, z);
    cmpi_net_flush(conn);
}

int cmpi_get_block_with_data(cmpi_connection_t *conn, int x, int y, int z, int *data)
{
    cmpi_net_printf(conn, "world.getBlock(%d,%d,%d)\n", x, y, z);
    cmpi_net_flush(conn);
}

void cmpi_set_block(cmpi_connection_t *conn, int x, int y, int z, int id)
{
    cmpi_net_printf(conn, "world.setBlock(%d,%d,%d,%d)\n", x, y, z, id);
    cmpi_net_flush(conn);
}
