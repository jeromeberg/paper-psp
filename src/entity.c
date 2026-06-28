#include "game.h"

void place_initial_area(int cx, int cy, uint8_t owner_id)
{
    for (int dr = -PLAYER_INIT_SZ/2; dr <= PLAYER_INIT_SZ/2; dr++) {
        for (int dc = -PLAYER_INIT_SZ/2; dc <= PLAYER_INIT_SZ/2; dc++) {
            int r = cy + dr, c = cx + dc;
            if (r >= 1 && r < GRID_ROWS-1 && c >= 1 && c < GRID_COLS-1) {
                g_grid[r][c] = CELL_ENCODE(owner_id, CTYPE_AREA);
                entity_adjust_area(owner_id, +1);
            }
        }
    }
}

void entity_adjust_area(uint8_t owner_id, int delta) {
    if (owner_id == OWNER_PLAYER) {
        g_game.player.area = (uint16_t)((int)g_game.player.area + delta);
    } else {
        int bi = (int)owner_id - (int)OWNER_BOT0;
        if (bi >= 0 && bi < MAX_BOTS)
            g_game.bots[bi].area = (uint16_t)((int)g_game.bots[bi].area + delta);
    }
}

void entity_kill(uint8_t owner_id)
{
    if (owner_id == OWNER_PLAYER) {
        g_game.player.alive = 0;
        g_game.state = GS_DEAD;
        return;
    }
    int bi = (int)owner_id - (int)OWNER_BOT0;
    if (bi < 0 || bi >= MAX_BOTS) return;
    Bot *b = &g_game.bots[bi];
    if (!b->alive) return;

    for (int r = 0; r < GRID_ROWS; r++)
        for (int c = 0; c < GRID_COLS; c++)
            if (CELL_OWNER(g_grid[r][c]) == owner_id) {
                if (CELL_TYPE(g_grid[r][c]) == CTYPE_TRAIL)
                    g_grid[r][c] = g_under[r][c];
                else
                    g_grid[r][c] = CELL_ENCODE(OWNER_NONE, CTYPE_EMPTY);
            }
    b->area = 0;
    b->alive = 0;
}
