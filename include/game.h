#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "render.h"

extern Cell         g_grid[GRID_ROWS][GRID_COLS];
extern Cell         g_under[GRID_ROWS][GRID_COLS];
extern Game         g_game;

/* --- game --- */
void     game_init(void);
void     game_update(uint32_t buttons);
uint32_t rng_next(void);

/* --- entity --- */
void entity_kill(uint8_t owner_id);
void entity_adjust_area(uint8_t owner_id, int delta);
void place_initial_area(int cx, int cy, uint8_t owner_id);

/* --- bot --- */
void spawn_bot(int slot);
void bot_init(Bot *b, int slot);
void bot_update(Bot *b);

/* --- input --- */
void handle_input(const SDL_Event *event, uint32_t *buttons, int *running);

/* --- utils --- */
void area_fill(uint8_t owner_id);
