#ifndef CMPI_DEF_H
#define CMPI_DEF_H


typedef enum {
    CMPI_STANDING_SIGN_S,
    CMPI_STANDING_SIGN_SSW,
    CMPI_STANDING_SIGN_SW,
    CMPI_STANDING_SIGN_WSW,
    CMPI_STANDING_SIGN_W,
    CMPI_STANDING_SIGN_WNW,
    CMPI_STANDING_SIGN_NW,
    CMPI_STANDING_SIGN_NNW,
    CMPI_STANDING_SIGN_N,
    CMPI_STANDING_SIGN_NNE,
    CMPI_STANDING_SIGN_NE,
    CMPI_STANDING_SIGN_ENE,
    CMPI_STANDING_SIGN_E,
    CMPI_STANDING_SIGN_ESE,
    CMPI_STANDING_SIGN_SE,
    CMPI_STANDING_SIGN_SSE
} cmpi_standing_sing_rotation_t;

typedef enum {
    CMPI_WALL_SIGN_NORTH = 2,
    CMPI_WALL_SIGN_SOUTH = 3,
    CMPI_WALL_SIGN_WEST  = 4,
    CMPI_WALL_SIGN_EAST  = 5,
} cmpi_wall_sign_direction_t;

#define CMPI_WORLD_SETTING_WORLD_IMMUTABLE  "world_immutable"
#define CMPI_WORLD_SETTING_NAMETAGS_VISIBLE "nametags_visible"

#define CMPI_MAX_ENTITY_TYPE_NAME 32

typedef struct {
    int id;
    int type_id;
    char type_name[CMPI_MAX_ENTITY_TYPE_NAME];
    double x, y, z;
} cmpi_world_entity_info_t;


#endif // CMPI_DEF_H
