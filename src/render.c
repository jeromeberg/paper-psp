#include "game.h"

static const SDL_Color COL_BORDER = {0xFF, 0xFF, 0xFF, 0xFF};
static const SDL_Color COL_BG     = {0x10, 0x10, 0x10, 0xFF};

static SDL_Color owner_color(uint8_t owner, int is_head_or_trail) 
{
    if (owner == OWNER_NONE) return COL_BG;
    int ci = (owner == OWNER_PLAYER) ? 0 : 1 + ((owner - OWNER_BOT0) % (NUM_PLAYER_COLORS - 1));
    return is_head_or_trail ? g_colors[ci].trail : g_colors[ci].area;
}

static void render_grid(SDL_Renderer *r, int cam_x, int cam_y) 
{
    int col_min = cam_x / CELL_SIZE;
    int col_max = (cam_x + SCR_WIDTH  + CELL_SIZE - 1) / CELL_SIZE;
    int row_min = cam_y / CELL_SIZE;
    int row_max = (cam_y + SCR_HEIGHT + CELL_SIZE - 1) / CELL_SIZE;
    if (col_min < 0)          col_min = 0;
    if (row_min < 0)          row_min = 0;
    if (col_max > GRID_COLS)  col_max = GRID_COLS;
    if (row_max > GRID_ROWS)  row_max = GRID_ROWS;

    int px = g_game.player.x;
    int py = g_game.player.y;

    for (int row = row_min; row < row_max; row++) {
        for (int col = col_min; col < col_max; col++) {
            SDL_Color c;

            if (row == 0 || row == GRID_ROWS-1 || col == 0 || col == GRID_COLS-1) {
                c = COL_BORDER;
            } else {
                Cell cell  = g_grid[row][col];
                uint8_t owner = CELL_OWNER(cell);
                uint8_t type  = CELL_TYPE(cell);

                int is_head = 0;
                uint8_t head_owner = 0;
                if (row == py && col == px && g_game.player.alive) {
                    is_head = 1;
                    head_owner = OWNER_PLAYER;
                } else {
                    for (int i = 0; i < g_game.num_bots; i++) {
                        const Bot *b = &g_game.bots[i];
                        if (b->alive && row == b->y && col == b->x) {
                            is_head = 1;
                            head_owner = (uint8_t)(OWNER_BOT0 + i);
                            break;
                        }
                    }
                }

                if (is_head) {
                    c = owner_color(head_owner, 1);
                } else if (owner == OWNER_NONE) {
                    continue;
                } else {
                    c = owner_color(owner, type != CTYPE_AREA);
                }
            }

            int sx = col * CELL_SIZE - cam_x;
            int sy = row * CELL_SIZE - cam_y;
            draw_set_color(r, c);
            draw_fill_rect(r, sx, sy, CELL_SIZE, CELL_SIZE);
        }
    }
}

static void render_hud(SDL_Renderer *r) 
{
    int area_pct = (GRID_TOTAL > 0) ? (int)g_game.player.area * 100 / GRID_TOTAL : 0;
    char buf[48];
    SDL_snprintf(buf, sizeof(buf), "KILLS:%d  AREA:%d%%", g_game.player.kills, area_pct);

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0x00, 0x00, 0x00, 0xA0);
    draw_fill_rect(r, 0, 0, (int)SDL_strlen(buf) * 8 + 6, 11);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    draw_set_color(r, COL_WHITE);
    draw_text(r, buf, 3, 2, 1);
}

void render_frame(SDL_Renderer *renderer) 
{
    SDL_SetRenderDrawColor(renderer, COL_BG.r, COL_BG.g, COL_BG.b, 0xFF);
    SDL_RenderClear(renderer);

    int cam_x = g_game.player.x * CELL_SIZE - SCR_WIDTH  / 2;
    int cam_y = g_game.player.y * CELL_SIZE - SCR_HEIGHT / 2;
    int max_cam_x = GRID_COLS * CELL_SIZE - SCR_WIDTH;
    int max_cam_y = GRID_ROWS * CELL_SIZE - SCR_HEIGHT;
    if (cam_x < 0) cam_x = 0;
    if (cam_y < 0) cam_y = 0;
    if (cam_x > max_cam_x) cam_x = max_cam_x;
    if (cam_y > max_cam_y) cam_y = max_cam_y;

    render_grid(renderer, cam_x, cam_y);

    if (g_game.state == GS_PLAYING)
        render_hud(renderer);

    switch (g_game.state) {
    case GS_HOME:   screen_home(renderer);   break;
    case GS_PAUSED: screen_paused(renderer); break;
    case GS_DEAD:   screen_dead(renderer);   break;
    case GS_WIN:    screen_win(renderer);    break;
    default: break;
    }

    SDL_RenderPresent(renderer);
}
