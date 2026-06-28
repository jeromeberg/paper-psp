#include "game.h"

static SDL_GameController *s_controller = NULL;

static void set_btn(uint32_t *b, uint32_t mask, int down) 
{
    if (down) *b |=  mask;
    else      *b &= ~mask;
}

void handle_input(const SDL_Event *event, uint32_t *buttons, int *running) 
{
    switch (event->type) 
    {
    case SDL_QUIT:
        *running = 0;
        break;

    case SDL_CONTROLLERDEVICEADDED:
        s_controller = SDL_GameControllerOpen(event->cdevice.which);
        break;
    case SDL_CONTROLLERDEVICEREMOVED:
        if (s_controller) {
            SDL_GameControllerClose(s_controller);
            s_controller = NULL;
        }
        break;

    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP: {
        int down = (event->type == SDL_CONTROLLERBUTTONDOWN);
        switch (event->cbutton.button) {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:    set_btn(buttons, BTN_UP,    down); break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:  set_btn(buttons, BTN_DOWN,  down); break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:  set_btn(buttons, BTN_LEFT,  down); break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: set_btn(buttons, BTN_RIGHT, down); break;
        case SDL_CONTROLLER_BUTTON_START:      set_btn(buttons, BTN_START, down); break;
        case SDL_CONTROLLER_BUTTON_A:          set_btn(buttons, BTN_X,     down); break;
        default: break;
        }
        break;
    }

    case SDL_KEYDOWN:
    case SDL_KEYUP: 
    {
        int down = (event->type == SDL_KEYDOWN);
        switch (event->key.keysym.sym) 
        {
        case SDLK_UP:     set_btn(buttons, BTN_UP,    down); break;
        case SDLK_DOWN:   set_btn(buttons, BTN_DOWN,  down); break;
        case SDLK_LEFT:   set_btn(buttons, BTN_LEFT,  down); break;
        case SDLK_RIGHT:  set_btn(buttons, BTN_RIGHT, down); break;
        case SDLK_RETURN: set_btn(buttons, BTN_START, down); break;
        case SDLK_z:      set_btn(buttons, BTN_X,     down); break;
        case SDLK_ESCAPE: *running = 0; break;
        default: break;
        }
        break;
    }
    }
}
