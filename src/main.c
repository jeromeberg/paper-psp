#include "game.h"

int main(int argc, char *argv[]) 
{
    (void)argc; 
    (void)argv;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

    SDL_Window *window = SDL_CreateWindow(
        "Paper",
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED,
        SCR_WIDTH, 
        SCR_HEIGHT, 
        0
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, 
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    game_init();

    uint32_t buttons = 0;
    uint32_t last_tick = SDL_GetTicks();
    int running = 1;

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event))
            handle_input(&event, &buttons, &running);

        uint32_t now = SDL_GetTicks();
        if (now - last_tick >= TICK_MS) {
            last_tick = now;
            game_update(buttons);
        }

        if (g_game.state == GS_QUIT)
            running = 0;

        render_frame(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
