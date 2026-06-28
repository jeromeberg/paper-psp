#pragma once

#include "types.h"

extern const PlayerColors g_colors[NUM_PLAYER_COLORS];

#define COL_WHITE  ((SDL_Color){0xFF,0xFF,0xFF,0xFF})
#define COL_GREY   ((SDL_Color){0x88,0x88,0x88,0xFF})
#define COL_DARK   ((SDL_Color){0x10,0x10,0x10,0xFF})

/* --- render --- */
void render_frame(SDL_Renderer *renderer);

/* --- draw --- */
void draw_set_color(SDL_Renderer *r, SDL_Color c);
void draw_fill_rect(SDL_Renderer *r, int x, int y, int w, int h);
void draw_outline_rect(SDL_Renderer *r, int x, int y, int w, int h);
void draw_overlay(SDL_Renderer *r, uint8_t alpha);
void draw_color_overlay(SDL_Renderer *r, uint8_t rr, uint8_t gg, uint8_t bb);
void draw_text(SDL_Renderer *r, const char *s, int x, int y, int scale);
int  draw_center_x(const char *s, int scale);
void draw_text_centered(SDL_Renderer *r, const char *s, int y, int scale);

/* --- screens --- */
void screen_home(SDL_Renderer *r);
void screen_paused(SDL_Renderer *r);
void screen_dead(SDL_Renderer *r);
void screen_win(SDL_Renderer *r);
