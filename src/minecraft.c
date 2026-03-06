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
    cmpi_net_printf(conn, "world.getBlockWithData(%d,%d,%d)\n", x, y, z);
    cmpi_net_flush(conn);
    cmpi_net_scanf(conn, "%d,%d", "\n", &id, data);
    return id;
}

void cmpi_get_blocks(cmpi_connection_t *conn, int x0, int y0, int z0, int x1, int y1, int z1, int *buffer)
{
    cmpi_net_printf(conn, "world.getBlocks(%d,%d,%d,%d,%d,%d)\n", x0, y0, z0, x1, y1, z1);
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

void cmpi_set_block_with_data(cmpi_connection_t *conn, int x, int y, int z, int id, int data)
{
    cmpi_net_printf(conn, "world.setBlock(%d,%d,%d,%d,%d)\n", x, y, z, id, data);
    cmpi_net_flush(conn);
}

void cmpi_set_blocks(cmpi_connection_t *conn, int x0, int y0, int z0, int x1, int y1, int z1, int id)
{
    cmpi_net_printf(conn, "world.setBlocks(%d,%d,%d,%d,%d,%d,%d)\n", x0, y0, z0, x1, y1, z1, id);
    cmpi_net_flush(conn);
}

void cmpi_set_blocks_with_data(cmpi_connection_t *conn, int x0, int y0, int z0, int x1, int y1, int z1, int id, int data)
{
    cmpi_net_printf(conn, "world.setBlocks(%d,%d,%d,%d,%d,%d,%d,%d)\n", x0, y0, z0, x1, y1, z1, id, data);
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

void cmpi_spawn_entity(cmpi_connection_t *conn, double x, double y, double z, int id)
{
    cmpi_net_printf(conn, "world.spawnEntity(%f,%f,%f,%d)\n", x, y, z, id);
    cmpi_net_flush(conn);
}

int cmpi_get_height(cmpi_connection_t *conn, int x, int z)
{
    int height = -1;
    cmpi_net_printf(conn, "world.getHeight(%d,%d)\n", x, z);
    cmpi_net_flush(conn);
    cmpi_net_scanf(conn, "%d", "\n", &height);
    return height;
}

int cmpi_get_players_count(cmpi_connection_t *conn)
{
    cmpi_net_printf(conn, "world.getPlayerIds()\n");
    cmpi_net_flush(conn);
    return cmpi_net_count_separators(conn, '|', '\n');
}

void cmpi_get_player_entity_ids(cmpi_connection_t *conn, int *buffer, size_t size)
{
    cmpi_net_printf(conn, "world.getPlayerIds()\n");
    cmpi_net_flush(conn);
    
    for(size_t i = 0; i < size; ++i)
    {
        int term = cmpi_net_scanf(conn, "%d", "|\n", &buffer[i]);
        if(term == '\n')
            return;
    }
}

int cmpi_get_player_entity_id_by_name(cmpi_connection_t *conn, const char *name)
{
    int id = -1;
    cmpi_net_printf(conn, "world.getPlayerId(%s)\n", name);
    cmpi_net_flush(conn);
    cmpi_net_scanf(conn, "%d", "\n", &id);
    return id;
}

void cmpi_save_world_checkpoint(cmpi_connection_t *conn)
{
    cmpi_net_printf(conn, "world.checkpoint.save()\n");
    cmpi_net_flush(conn);
}

void cmpi_restore_world_checkpoint(cmpi_connection_t *conn)
{
    cmpi_net_printf(conn, "world.checkpoint.restore()\n");
    cmpi_net_flush(conn);
}

void cmpi_post_to_chat(cmpi_connection_t *conn, const char *message)
{
    cmpi_net_printf(conn, "chat.post(%s)\n", message);
    cmpi_net_flush(conn);
}

void cmpi_world_setting(cmpi_connection_t *conn, const char *setting, int status)
{
    cmpi_net_printf(conn, "world.setting(%s,%d)\n", setting, status ? 1 : 0);
    cmpi_net_flush(conn);
}

int cmpi_get_entity_types_count(cmpi_connection_t *conn)
{
    cmpi_net_printf(conn, "world.getEntityTypes()\n");
    cmpi_net_flush(conn);
    return cmpi_net_count_separators(conn, '|', '\n');
}

void cmpi_get_entity_types(
    cmpi_connection_t *conn,
    int *type_ids,
    char names[][CMPI_MAX_ENTITY_TYPE_NAME],
    size_t size
)
{
    cmpi_net_printf(conn, "world.getEntityTypes()\n");
    cmpi_net_flush(conn);
    
    for(size_t i = 0; i < size; ++i)
    {
        int term = cmpi_net_scanf(
            conn, "%d,%s", "|\n", &type_ids[i], names[i], CMPI_MAX_ENTITY_TYPE_NAME
        );
        if(term == '\n')
            return;
    }
}

int cmpi_get_entities_count(cmpi_connection_t *conn)
{
    cmpi_net_printf(conn, "world.getEntities()\n");
    cmpi_net_flush(conn);
    return cmpi_net_count_separators(conn, '|', '\n');
}

void cmpi_get_entities(cmpi_connection_t *conn, cmpi_world_entity_info_t *buffer, size_t size)
{
    cmpi_net_printf(conn, "world.getEntities(%d)\n", -1);
    cmpi_net_flush(conn);
    
    for(size_t i = 0; i < size; ++i)
    {
        int term = cmpi_net_scanf(
            conn, "%d,%d,%s,%f,%f,%f", "|\n", 
            &buffer[i].id,
            &buffer[i].type_id,
            &buffer[i].type_name, CMPI_MAX_ENTITY_TYPE_NAME,
            &buffer[i].x,
            &buffer[i].y,
            &buffer[i].z
        );
        if(term == '\n')
            return;
    }
}

int cmpi_remove_entity_by_id(cmpi_connection_t *conn, int id)
{
    int result = -1;
    cmpi_net_printf(conn, "world.removeEntity(%d)\n", id);
    cmpi_net_flush(conn);
    cmpi_net_scanf(conn, "%d", "\n", &result);
    return result;
}

int cmpi_remove_entities_by_type(cmpi_connection_t *conn, int type_id)
{
    int count = -1;
    cmpi_net_printf(conn, "world.removeEntities(%d)\n", type_id);
    cmpi_net_flush(conn);
    cmpi_net_scanf(conn, "%d", "\n", &count);
    return count;
}
