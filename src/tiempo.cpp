#pragma once
#include <SDL2/SDL.h>

namespace tiempo {
    namespace {
        Uint32 lastTick = 0;
    }
    float DELTA_TIME = 0;

    void inicializarDeltaTime() {
        lastTick = SDL_GetTicks();
    }

    void setDeltaTime() {
        Uint32 now = SDL_GetTicks();
        DELTA_TIME = (now - lastTick) / 1000.0f;
        lastTick = now;
    }
}