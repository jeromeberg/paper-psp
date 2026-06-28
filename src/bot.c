#include "game.h"

void bot_init(Bot *b, int slot)
{
    (void)slot;
    b->aggressiveness = (uint8_t)BOT_AGGRESSIVENESS;
    b->greediness     = (uint8_t)BOT_GREEDINESS;
    b->explore_max    = (uint8_t)(MIN_RANGE * (50 + (int)b->greediness) / 50);
}

/* --- spawn --- */

static int valid_spawn(int cx, int cy)
{
    if (cx < 5 || cx >= GRID_COLS-5 || cy < 5 || cy >= GRID_ROWS-5) return 0;
    int dx = cx - (int)g_game.player.x;
    int dy = cy - (int)g_game.player.y;
    if (abs(dx) + abs(dy) < 20) return 0;
    for (int dr = -2; dr <= 2; dr++)
        for (int dc = -2; dc <= 2; dc++)
            if (g_grid[cy + dr][cx + dc] != 0) return 0;
    return 1;
}

void spawn_bot(int slot)
{
    uint8_t owner_id = (uint8_t)(OWNER_BOT0 + slot);

    for (int r = 0; r < GRID_ROWS; r++)
        for (int c = 0; c < GRID_COLS; c++)
            if (CELL_OWNER(g_grid[r][c]) == owner_id)
                g_grid[r][c] = CELL_ENCODE(OWNER_NONE, CTYPE_EMPTY);

    int cx, cy, attempts = 0;
    do {
        cx = 5 + (int)(rng_next() % (uint32_t)(GRID_COLS - 10));
        cy = 5 + (int)(rng_next() % (uint32_t)(GRID_ROWS - 10));
        attempts++;
    } while (!valid_spawn(cx, cy) && attempts < 200);

    if (attempts >= 200) return;

    Bot *b = &g_game.bots[slot];
    memset(b, 0, sizeof(*b));
    b->x = b->home_x = (int16_t)cx;
    b->y = b->home_y = (int16_t)cy;
    b->dir   = (Direction)(rng_next() % 4);
    b->alive = 1;
    b->state = BOT_EXPLORE;
    bot_init(b, slot);
    place_initial_area(cx, cy, owner_id);
}

/* --- direction --- */

static const int s_dx[4] = { 0,  1,  0, -1 };
static const int s_dy[4] = {-1,  0,  1,  0 };

static int       bot_col(Direction d) { return s_dx[d]; }
static int       bot_row(Direction d) { return s_dy[d]; }
static Direction dir_cw(Direction d)  { return (Direction)((d + 1) & 3); }
static Direction dir_ccw(Direction d) { return (Direction)((d + 3) & 3); }
static Direction dir_rev(Direction d) { return (Direction)((d + 2) & 3); }

static Direction dir_toward(int fx, int fy, int tx, int ty)
{
    int dr = ty - fy, dc = tx - fx;
    return (abs(dr) >= abs(dc))
        ? (dr < 0 ? DIR_UP : DIR_DOWN)
        : (dc < 0 ? DIR_LEFT : DIR_RIGHT);
}

/* --- movement --- */

static int safe_for(int r, int c, uint8_t owner_id)
{
    if (r < 1 || r >= GRID_ROWS-1 || c < 1 || c >= GRID_COLS-1) return 0;
    Cell cell = g_grid[r][c];
    return !(CELL_OWNER(cell) == owner_id && CELL_TYPE(cell) == CTYPE_TRAIL);
}

static int bot_move(Bot *b, uint8_t owner_id)
{
    Direction tries[4] = {b->dir, dir_cw(b->dir), dir_ccw(b->dir), dir_rev(b->dir)};
    int nr = -1, nc = -1;
    for (int i = 0; i < 4; i++) {
        int tr = b->y + bot_row(tries[i]);
        int tc = b->x + bot_col(tries[i]);
        if (safe_for(tr, tc, owner_id)) {
            b->dir = tries[i]; nr = tr; nc = tc; break;
        }
    }
    if (nr < 0) return 0;
    Cell cur = g_grid[b->y][b->x];
    if (!(CELL_OWNER(cur) == owner_id && CELL_TYPE(cur) == CTYPE_AREA)) {
        g_under[b->y][b->x] = cur;
        g_grid[b->y][b->x]  = CELL_ENCODE(owner_id, CTYPE_TRAIL);
        b->trail_len++;
    }
    b->x = (int16_t)nc;
    b->y = (int16_t)nr;
    return 1;
}

static void check_landing(Bot *b, uint8_t owner_id)
{
    Cell    landed = g_grid[b->y][b->x];
    uint8_t lo = CELL_OWNER(landed);
    uint8_t lt = CELL_TYPE(landed);

    if (lt == CTYPE_TRAIL) {
        if (lo == owner_id) 
            entity_kill(owner_id);
        else if (lo != OWNER_NONE) 
            entity_kill(lo);
        return;
    }
    if (lo == owner_id && lt == CTYPE_AREA) {
        area_fill(owner_id);
        b->home_x    = b->x;
        b->home_y    = b->y;
        b->trail_len = 0;
        b->state     = BOT_EXPLORE;
    }
}

/* --- area estimate --- */

static int potential_area(const Bot *b)
{
    int leg1 = b->explore_max / 2;
    int leg2 = (int)b->trail_len - leg1;
    return leg2 > 0 ? leg1 * leg2 : 0;
}

static int area_is_enough(const Bot *b)
{
    int leg = b->explore_max / 2;
    return potential_area(b) >= leg * leg;
}

/* --- behaviours --- */

/* hunt nearest enemy */
static int hunt_nearest(Bot *b, uint8_t owner_id)
{
    int range = MIN_RANGE * (50 + (int)b->aggressiveness) / 50;
    if ((int)(rng_next() % 100) >= (int)b->aggressiveness) return 0;

    int best = range + 1, tr = -1, tc = -1;

    if (g_game.player.alive) {
        int d = abs((int)g_game.player.y - b->y) + abs((int)g_game.player.x - b->x);
        if (d < best) { best = d; tr = g_game.player.y; tc = g_game.player.x; }
    }
    for (int i = 0; i < g_game.num_bots; i++) {
        const Bot *o = &g_game.bots[i];
        if (!o->alive || o == b || o->trail_len == 0) continue;
        int d = abs((int)o->y - b->y) + abs((int)o->x - b->x);
        if (d < best) { best = d; tr = o->y; tc = o->x; }
    }
    if (tr < 0) return 0;

    Direction pref = dir_toward(b->x, b->y, tc, tr);
    Direction tries[4] = {pref, dir_cw(pref), dir_ccw(pref), dir_rev(pref)};
    for (int t = 0; t < 4; t++) {
        int nr = b->y + bot_row(tries[t]);
        int nc = b->x + bot_col(tries[t]);
        if (safe_for(nr, nc, owner_id)) { b->dir = tries[t]; return 1; }
    }
    return 0;
}

static int find_nearest_own_area(const Bot *b, uint8_t owner_id, int *tx, int *ty)
{
    int best = GRID_ROWS + GRID_COLS, bx = -1, by = -1;
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (CELL_OWNER(g_grid[r][c]) != owner_id) continue;
            if (CELL_TYPE(g_grid[r][c]) != CTYPE_AREA) continue;
            int dist = abs(c - (int)b->x) + abs(r - (int)b->y);
            if (dist < best) { best = dist; bx = c; by = r; }
        }
    }
    if (bx < 0) return 0;
    *tx = bx; *ty = by;
    return 1;
}

/* return to nearest owned area cell */
static int go_home(Bot *b, uint8_t owner_id)
{
    int tx, ty;
    if (!find_nearest_own_area(b, owner_id, &tx, &ty)) return 0;

    int dr = ty - (int)b->y;
    int dc = tx - (int)b->x;

    /* keep current direction while it still closes distance on its axis;
       only switch to the other axis once the first is resolved */
    int cur_ok = (b->dir == DIR_UP    && dr < 0)
              || (b->dir == DIR_DOWN  && dr > 0)
              || (b->dir == DIR_LEFT  && dc < 0)
              || (b->dir == DIR_RIGHT && dc > 0);
    if (!cur_ok)
        b->dir = dir_toward(b->x, b->y, tx, ty);

    int nr = b->y + bot_row(b->dir);
    int nc = b->x + bot_col(b->dir);
    if (!safe_for(nr, nc, owner_id)) {
        Direction tries[3] = {dir_toward(b->x, b->y, tx, ty),
                              dir_cw(b->dir), dir_ccw(b->dir)};
        int ok = 0;
        for (int i = 0; i < 3; i++) {
            nr = b->y + bot_row(tries[i]);
            nc = b->x + bot_col(tries[i]);
            if (safe_for(nr, nc, owner_id)) { b->dir = tries[i]; ok = 1; break; }
        }
        if (!ok) return 0;
    }
    return 1;
}

static void explore(Bot *b, uint8_t owner_id)
{
    if ((int)b->trail_len != b->explore_max / 2) return;

    Direction cw  = dir_cw(b->dir);
    Direction ccw = dir_ccw(b->dir);
    int cw_empty = 0, ccw_empty = 0;
    for (int s = 1; s <= 8; s++) {
        int r, c;
        r = b->y + bot_row(cw)  * s; c = b->x + bot_col(cw)  * s;
        if (r < 1 || r >= GRID_ROWS-1 || c < 1 || c >= GRID_COLS-1 || CELL_TYPE(g_grid[r][c]) != CTYPE_EMPTY) break;
        cw_empty++;
    }
    for (int s = 1; s <= 8; s++) {
        int r, c;
        r = b->y + bot_row(ccw) * s; c = b->x + bot_col(ccw) * s;
        if (r < 1 || r >= GRID_ROWS-1 || c < 1 || c >= GRID_COLS-1 || CELL_TYPE(g_grid[r][c]) != CTYPE_EMPTY) break;
        ccw_empty++;
    }

    Direction pick = (cw_empty >= ccw_empty) ? cw : ccw;
    Direction alt  = (pick == cw) ? ccw : cw;
    int nr = b->y + bot_row(pick), nc = b->x + bot_col(pick);
    if (safe_for(nr, nc, owner_id)) { b->dir = pick; return; }
    nr = b->y + bot_row(alt); nc = b->x + bot_col(alt);
    if (safe_for(nr, nc, owner_id)) b->dir = alt;
}

/* --- update --- */

static uint8_t bot_owner(const Bot *b)
{
    for (int i = 0; i < g_game.num_bots; i++)
        if (&g_game.bots[i] == b)
            return (uint8_t)(OWNER_BOT0 + i);
    return OWNER_BOT0;
}

void bot_update(Bot *b)
{
    uint8_t owner_id = bot_owner(b);

    int too_far = abs((int)b->x - (int)b->home_x) + abs((int)b->y - (int)b->home_y)
                  > b->explore_max + MIN_RANGE;

    if (b->state == BOT_RETURN) {
        if (!hunt_nearest(b, owner_id))
            if (!go_home(b, owner_id)) return;

    } else { /* BOT_EXPLORE */
        if (hunt_nearest(b, owner_id)) {
            /* hunting overrides explore this tick */
        } else if (area_is_enough(b) || too_far) {
            b->state = BOT_RETURN;
            if (!go_home(b, owner_id)) return;
        } else {
            explore(b, owner_id);
        }
    }

    if (!bot_move(b, owner_id)) return;
    check_landing(b, owner_id);
}
