#pragma once

#include <stdint.h>
#include <SDL.h>
#include "config.h"

typedef enum {
    DIR_UP      = 0,
    DIR_RIGHT   = 1,
    DIR_DOWN    = 2,
    DIR_LEFT    = 3
} Direction;

typedef enum {
    GS_HOME     = 0,
    GS_PLAYING  = 1,
    GS_PAUSED   = 2,
    GS_DEAD     = 3,
    GS_WIN      = 4,
    GS_QUIT     = 5
} GameState;

typedef enum {
    BOT_EXPLORE = 0,
    BOT_RETURN  = 1
} BotState;

typedef uint8_t Cell;

typedef enum {
    OWNER_NONE   = 0,
    OWNER_PLAYER = 1,
    OWNER_BOT0   = 2
} Owner;

typedef enum {
    CTYPE_EMPTY = 0,
    CTYPE_AREA  = 1,
    CTYPE_TRAIL = 2
} Ctype;

#define CELL_ENCODE(owner, type)  ((uint8_t)(((owner) << 4) | ((type) & 0x0F)))
#define CELL_OWNER(c)             ((uint8_t)(((c) >> 4) & 0x0F))
#define CELL_TYPE(c)              ((uint8_t)((c) & 0x0F))

typedef struct {
    SDL_Color area;
    SDL_Color trail;
} PlayerColors;

typedef struct {
    int16_t   x, y;
    Direction dir;
    uint8_t   alive;
    uint16_t  area;
    int       kills;
    int       largest_area;
} Player;

typedef struct {
    int16_t   x, y;
    int16_t   home_x, home_y;
    Direction dir;
    uint8_t   alive;
    uint8_t   trail_len;
    uint8_t   explore_max;
    BotState  state;
    uint16_t  area;
    uint8_t   aggressiveness;
    uint8_t   greediness;
} Bot;

typedef struct {
    GameState state;
    Player    player;
    Bot       bots[MAX_BOTS];
    int       num_bots;
    uint32_t  tick;
    int8_t    pause_sel;
    uint32_t  prev_buttons;
} Game;
