#pragma once

#define SCR_WIDTH   480
#define SCR_HEIGHT  272

#define CELL_SIZE        8
#define GRID_COLS        120
#define GRID_ROWS        68
#define GRID_TOTAL       (GRID_COLS * GRID_ROWS)

#define PLAYER_START_X   (GRID_COLS / 2)
#define PLAYER_START_Y   (GRID_ROWS / 2)
#define PLAYER_INIT_SZ   3

#define NUM_PLAYER_COLORS   20
#define DEFAULT_NUM_BOTS    12
#define MAX_BOTS            13
#define BOT_AGGRESSIVENESS  25
#define BOT_GREEDINESS      30
#define MIN_RANGE           8
#define MIN_ALIVE_BOTS      7

#define TICK_MS   100

#define BTN_UP      (1u << 0)
#define BTN_DOWN    (1u << 1)
#define BTN_LEFT    (1u << 2)
#define BTN_RIGHT   (1u << 3)
#define BTN_START   (1u << 4)
#define BTN_X       (1u << 5)
#define BTN_CONFIRM (BTN_START | BTN_X)
