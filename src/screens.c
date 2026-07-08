#include "game.h"

void screen_home(SDL_Renderer *r) 
{
    draw_overlay(r, 160);

    draw_set_color(r, COL_WHITE);
    draw_text_centered(r, "PAPER PSP", 90, 3);

    draw_set_color(r, COL_GREY);
    draw_text_centered(r, "PRESS START TO PLAY", 170, 2);
}

void screen_paused(SDL_Renderer *r) 
{
    draw_overlay(r, 170);

    int pw = 200, ph = 116;
    int px = (SCR_WIDTH - pw) / 2;
    int py = (SCR_HEIGHT - ph) / 2;

    SDL_SetRenderDrawColor(r, 0x1A, 0x1A, 0x28, 0xFF);
    draw_fill_rect(r, px, py, pw, ph);
    draw_set_color(r, COL_GREY);
    draw_outline_rect(r, px, py, pw, ph);

    draw_set_color(r, COL_WHITE);
    draw_text_centered(r, "PAUSED", py + 12, 2);

    const char *opts[3] = {"RESUME", "RESTART", "EXIT"};
    for (int i = 0; i < 3; i++) {
        int oy = py + 48 + i * 24;
        int tx = draw_center_x(opts[i], 2);
        int tw = (int)strlen(opts[i]) * 16;

        if (i == g_game.pause_sel) {
            SDL_SetRenderDrawColor(r, 0xFF, 0xFF, 0xFF, 0xFF);
            draw_fill_rect(r, tx - 8, oy - 2, tw + 16, 20);
            draw_set_color(r, COL_DARK);
        } else {
            draw_set_color(r, COL_GREY);
        }
        draw_text(r, opts[i], tx, oy, 2);
    }
}

static void draw_stats(SDL_Renderer *r, int y) 
{
    char buf[48];
    int best_pct = g_game.player.largest_area * 100 / GRID_TOTAL;

    snprintf(buf, sizeof(buf), "KILLS: %d", g_game.player.kills);
    draw_set_color(r, COL_WHITE);
    draw_text_centered(r, buf, y, 1);

    snprintf(buf, sizeof(buf), "BEST AREA: %d%%", best_pct);
    draw_text_centered(r, buf, y + 12, 1);
}

void screen_dead(SDL_Renderer *r) 
{
    draw_color_overlay(r, 0xFF, 0x00, 0x00);

    draw_set_color(r, COL_WHITE);
    draw_text_centered(r, "GAME OVER", 84, 3);

    draw_set_color(r, COL_GREY);
    draw_stats(r, 148);
    draw_text_centered(r, "PRESS START TO RETRY", 176, 2);
}

void screen_win(SDL_Renderer *r) 
{
    draw_color_overlay(r, 0x00, 0xCC, 0x00);

    draw_set_color(r, COL_WHITE);
    draw_text_centered(r, "YOU WIN!", 84, 3);

    draw_set_color(r, COL_GREY);
    draw_stats(r, 148);
    draw_text_centered(r, "PRESS START TO RETRY", 176, 2);
}
