#pragma once
#include <SDL2/SDL.h>

class Sprite {
public:
    int posX, posY;
    int movX, movY;
    float scale;
    SDL_Rect spriteRect;
    SDL_Surface* sprite;

    Sprite(int _posX, int _posY, int _movX, int _movY, float _scale, SDL_Surface* _sprite);

    void mover(int winW, int winH);

private:
    void actualizarRect();
};
