#ifndef CMPI_MINECRAFT_H
#define CMPI_MINECRAFT_H

#include <stddef.h>
#include "net.h"
#include "def.h"

#ifdef __cplusplus
extern "C" {
#endif


/** Returns ID of block at coords. */
int cmpi_get_block(cmpi_connection_t *conn, int x, int y, int z);

/** Returns ID of block at coords and sets data to block data. */
int cmpi_get_block_with_data(cmpi_connection_t *conn, int x, int y, int z, int *data);

/** Fills buffer with IDs of blocks in cuboid in order X->Z->Y. (RaspberryJuice extension) */
void cmpi_get_blocks(
    cmpi_connection_t *conn, 
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    int *buffer
);

/** Sets block ID at coords. */
void cmpi_set_block(cmpi_connection_t *conn, int x, int y, int z, int id);

/** Sets block ID at coords with data. */
void cmpi_set_block_with_data(cmpi_connection_t *conn, int x, int y, int z, int id, int data);

/** Fills cuboid with block ID. */
void cmpi_set_blocks(
    cmpi_connection_t *conn, 
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    int id
);

/** Fills cuboid with block ID and data. */
void cmpi_set_blocks_with_data(
    cmpi_connection_t *conn, 
    int x0, int y0, int z0, 
    int x1, int y1, int z1, 
    int id, int data
);

/** Sets a standing sign (ID 63) with rotation and 4 lines of text. (RaspberryJuice extension) */
void cmpi_set_standing_sign(
    cmpi_connection_t *conn, 
    int x, int y, int z,
    cmpi_standing_sing_rotation_t rotation,
    const char *line1,
    const char *line2,
    const char *line3,
    const char *line4
);

/** Sets a wall sign (ID 68) with direction and 4 lines of text. (RaspberryJuice extension) */
void cmpi_set_wall_sign(
    cmpi_connection_t *conn, 
    int x, int y, int z,
    cmpi_wall_sign_direction_t direction,
    const char *line1,
    const char *line2,
    const char *line3,
    const char *line4
);

/** Spawns an entity with ID at coords. (RaspberryJuice extension) */
void cmpi_spawn_entity(cmpi_connection_t *conn, double x, double y, double z, int id);

/** Returns height (max non-air Y) at XZ plain coords. */
int cmpi_get_height(cmpi_connection_t *conn, int x, int z);

/** Returns players count in the world. */
int cmpi_get_players_count(cmpi_connection_t *conn);

/** Fills buffer with connected players entity IDs. */
void cmpi_get_player_entity_ids(cmpi_connection_t *conn, int *buffer, size_t size);

/** Returns entity ID of player with name, or -1 if there's no player with name. */
int cmpi_get_player_entity_id_by_name(cmpi_connection_t *conn, const char *name);

/** Saves the current state of the Minecraft world as a checkpoint. */
void cmpi_save_world_checkpoint(cmpi_connection_t *conn);

/** Restores the world state to the last saved checkpoint. */
void cmpi_restore_world_checkpoint(cmpi_connection_t *conn);

/** Posts message to game chat. */
void cmpi_post_to_chat(cmpi_connection_t *conn, const char *message);

/** Sets a world setting. */
void cmpi_world_setting(cmpi_connection_t *conn, const char *setting, int status);

/** Returns count of entity types available in connected Minecraft. */
int cmpi_get_entity_types_count(cmpi_connection_t *conn);

/** Stores entity types available in connected Minecraft. */
void cmpi_get_entity_types(
    cmpi_connection_t *conn, 
    int *type_ids, 
    char names[][CMPI_MAX_ENTITY_TYPE_NAME], 
    size_t size
);

/** Returns count of entities in the world. (RaspberryJuice extension) */
int cmpi_get_entities_count(cmpi_connection_t *conn);

/** Fills buffer with all world entities data. (RaspberryJuice extension) */
void cmpi_get_entities(cmpi_connection_t *conn, cmpi_world_entity_info_t *buffer, size_t size);

/** Returns whether was entity with ID removed or there's no entity with ID. (RaspberryJuice extension) */
int cmpi_remove_entity_by_id(cmpi_connection_t *conn, int id);

/** Returns count of removed entities with type ID. (RaspberryJuice extension) */
int cmpi_remove_entities_by_type(cmpi_connection_t *conn, int type_id);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // CMPI_MINECRAFT_H
