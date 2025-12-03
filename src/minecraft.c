#include "minecraft.h"

int cmpi_get_block(cmpi_connection_t *conn, int x, int y, int z)
{
    int id = -1;
    cmpi_net_printf(conn, "world.getBlock(%d,%d,%d)\n", x, y, z);
    cmpi_net_flush(conn);
    cmpi_net_scanf(conn, "%d", "\n", &id);
    return id;
}

int cmpi_get_block_with_data(cmpi_connection_t *conn, int x, int y, int z, int *data)
{
    int id = -1;
    cmpi_net_printf(conn, "world.getBlock(%d,%d,%d)\n", x, y, z);
    cmpi_net_flush(conn);
    cmpi_net_scanf(conn, "%d,%d", "\n", &id, data);
    return id;
}

void cmpi_get_blocks(cmpi_connection_t *conn, int x0, int y0, int z0, int x1, int y1, int z1, int *buffer)
{
    cmpi_net_printf(conn, "world.getBlocks(%d,%d,%d,%d,%d,%d)\n", x0, y0, z0, x1, y1, x1);
    cmpi_net_flush(conn);

    for(int term;; ++buffer)
    {
        int term = cmpi_net_scanf(conn, "%d", ",\n", buffer);
        if(term == '\n')
            return;
    }
}

void cmpi_set_block(cmpi_connection_t *conn, int x, int y, int z, int id)
{
    cmpi_net_printf(conn, "world.setBlock(%d,%d,%d,%d)\n", x, y, z, id);
    cmpi_net_flush(conn);
}

void cmpi_set_blocks(cmpi_connection_t *conn, int x0, int y0, int z0, int x1, int y1, int z1, int id)
{
    cmpi_net_printf(conn, "world.setBlock(%d,%d,%d,%d,%d,%d,%d)\n", x0, y0, z0, x1, y1, z1, id);
    cmpi_net_flush(conn);
}

void cmpi_set_blocks_with_data(cmpi_connection_t *conn, int x0, int y0, int z0, int x1, int y1, int z1, int id, int data)
{
    cmpi_net_printf(conn, "world.setBlock(%d,%d,%d,%d,%d,%d,%d,%d)\n", x0, y0, z0, x1, y1, z1, id, data);
    cmpi_net_flush(conn);
}

void cmpi_set_standing_sign(cmpi_connection_t *conn, int x, int y, int z, cmpi_standing_sing_rotation_t rotation, const char *line1, const char *line2, const char *line3, const char *line4)
{
    cmpi_net_printf(
        conn, "world.setSign(%d,%d,%d,63,%d,%s,%s,%s,%s)\n", 
        x, y, z, rotation, line1, line2, line3, line4
    );
    cmpi_net_flush(conn);
}

void cmpi_set_wall_sign(cmpi_connection_t *conn, int x, int y, int z, cmpi_wall_sign_direction_t direction, const char *line1, const char *line2, const char *line3, const char *line4)
{
    cmpi_net_printf(
        conn, "world.setSign(%d,%d,%d,68,%d,%s,%s,%s,%s)\n", 
        x, y, z, direction, line1, line2, line3, line4
    );
    cmpi_net_flush(conn);
}
