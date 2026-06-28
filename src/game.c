#include "game.h"

Cell g_grid[GRID_ROWS][GRID_COLS];
Cell g_under[GRID_ROWS][GRID_COLS];
Game g_game;

static uint32_t g_rng;

uint32_t rng_next(void)
{
    g_rng = g_rng * 1664525u + 1013904223u;
    return g_rng;
}

/* --- direction --- */

static const int g_dx[4] = { 0,  1,  0, -1 };
static const int g_dy[4] = {-1,  0,  1,  0 };

static int dir_col(Direction d) { return g_dx[d]; }
static int dir_row(Direction d) { return g_dy[d]; }

/* --- bots --- */

static int count_alive_bots(void)
{
    int n = 0;
    for (int i = 0; i < MAX_BOTS; i++)
        if (g_game.bots[i].alive) n++;
    return n;
}

static int find_dead_slot(void)
{
    for (int i = 0; i < MAX_BOTS; i++)
        if (!g_game.bots[i].alive) return i;
    return -1;
}

static void game_maybe_spawn_bots(void) {
    if (count_alive_bots() >= MIN_ALIVE_BOTS) return;
    int slot = find_dead_slot();
    if (slot >= 0) spawn_bot(slot);
}

/* --- player --- */

static Direction input_to_dir(uint32_t buttons, Direction cur)
{
    Direction nd = cur;
    if (buttons & BTN_UP)    nd = DIR_UP;
    if (buttons & BTN_RIGHT) nd = DIR_RIGHT;
    if (buttons & BTN_DOWN)  nd = DIR_DOWN;
    if (buttons & BTN_LEFT)  nd = DIR_LEFT;
    if ((cur == DIR_UP    && nd == DIR_DOWN)  ||
        (cur == DIR_DOWN  && nd == DIR_UP)    ||
        (cur == DIR_LEFT  && nd == DIR_RIGHT) ||
        (cur == DIR_RIGHT && nd == DIR_LEFT))
        nd = cur;
    return nd;
}

static void player_step(void)
{
    Player *p = &g_game.player;
    int nr = p->y + dir_row(p->dir);
    int nc = p->x + dir_col(p->dir);

    if (nr < 1 || nr >= GRID_ROWS-1 || nc < 1 || nc >= GRID_COLS-1) {
        entity_kill(OWNER_PLAYER);
        return;
    }

    Cell next = g_grid[nr][nc];
    uint8_t next_owner = CELL_OWNER(next);
    uint8_t next_type  = CELL_TYPE(next);

    if (next_owner == OWNER_PLAYER && next_type == CTYPE_TRAIL) {
        entity_kill(OWNER_PLAYER);
        return;
    }

    if (next_type == CTYPE_TRAIL && next_owner != OWNER_NONE) {
        entity_kill(next_owner);
        g_game.player.kills++;
    }

    Cell cur = g_grid[p->y][p->x];
    if (!(CELL_OWNER(cur) == OWNER_PLAYER && CELL_TYPE(cur) == CTYPE_AREA)) {
        g_under[p->y][p->x] = cur;
        g_grid[p->y][p->x] = CELL_ENCODE(OWNER_PLAYER, CTYPE_TRAIL);
    }

    p->x = (int16_t)nc;
    p->y = (int16_t)nr;

    Cell landed = g_grid[nr][nc];
    if (CELL_OWNER(landed) == OWNER_PLAYER && CELL_TYPE(landed) == CTYPE_AREA) {
        area_fill(OWNER_PLAYER);
        if ((int)g_game.player.area > g_game.player.largest_area)
            g_game.player.largest_area = (int)g_game.player.area;
        if (g_game.player.area * 2 >= GRID_TOTAL)
            g_game.state = GS_WIN;
    }
}

/* --- game --- */

static const int16_t s_bot_spawn[MAX_BOTS][2] = {
    {10, 10}, {60, 10}, {110, 10},
    {10, 34},           {110, 34},
    {10, 58}, {60, 58}, {110, 58},
    {35, 22},           {85, 22},
    {35, 46},           {85, 46},
    {60, 22},
};

void game_init(void)
{
    uint64_t seed = SDL_GetPerformanceCounter();
    g_rng = (uint32_t)(seed ^ (seed >> 32));

    memset(g_grid,  0, sizeof(g_grid));
    memset(g_under, 0, sizeof(g_under));
    memset(&g_game, 0, sizeof(g_game));

    g_game.state    = GS_HOME;
    g_game.num_bots = MAX_BOTS;

    g_game.player.x     = PLAYER_START_X;
    g_game.player.y     = PLAYER_START_Y;
    g_game.player.dir   = DIR_RIGHT;
    g_game.player.alive = 1;
    place_initial_area(PLAYER_START_X, PLAYER_START_Y, OWNER_PLAYER);

    for (int i = 0; i < MAX_BOTS; i++) {
        Bot *b = &g_game.bots[i];
        memset(b, 0, sizeof(*b));
        if (i < DEFAULT_NUM_BOTS) {
            b->x = b->home_x = s_bot_spawn[i][0];
            b->y = b->home_y = s_bot_spawn[i][1];
            b->dir   = DIR_DOWN;
            b->alive = 1;
            b->state = BOT_EXPLORE;
            bot_init(b, i);
            place_initial_area(b->x, b->y, (uint8_t)(OWNER_BOT0 + i));
        }
    }
}

void game_update(uint32_t buttons)
{
    uint32_t pressed = buttons & ~g_game.prev_buttons;
    g_game.prev_buttons = buttons;

    switch (g_game.state)
    {
    case GS_HOME:
        if (pressed & BTN_CONFIRM)
            g_game.state = GS_PLAYING;
        break;

    case GS_PLAYING:
        g_game.tick++;

        if (g_game.tick % 120 == 0)
            game_maybe_spawn_bots();

        if (pressed & BTN_START) {
            g_game.state = GS_PAUSED;
            g_game.pause_sel = 0;
            break;
        }
        g_game.player.dir = input_to_dir(buttons, g_game.player.dir);
        if (g_game.player.alive)
            player_step();

        for (int i = 0; i < g_game.num_bots; i++)
            if (g_game.bots[i].alive)
                bot_update(&g_game.bots[i]);
        break;

    case GS_PAUSED:
        if (pressed & BTN_UP)
            g_game.pause_sel = (int8_t)((g_game.pause_sel + 2) % 3);
        if (pressed & BTN_DOWN)
            g_game.pause_sel = (int8_t)((g_game.pause_sel + 1) % 3);
        if (pressed & BTN_CONFIRM) {
            switch (g_game.pause_sel) {
            case 0:
                g_game.state = GS_PLAYING;
                break;
            case 1: {
                uint32_t cur = buttons;
                game_init();
                g_game.state = GS_PLAYING;
                g_game.prev_buttons = cur;
                break;
            }
            case 2:
                g_game.state = GS_QUIT;
                break;
            }
        }
        break;

    case GS_DEAD:
    case GS_WIN:
        if (pressed & BTN_CONFIRM) {
            uint32_t cur = buttons;
            game_init();
            g_game.state = GS_PLAYING;
            g_game.prev_buttons = cur;
        }
        break;

    case GS_QUIT:
        break;
    }
}
