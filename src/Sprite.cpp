#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

class Sprite {
    public:
        int posX;
        int posY;
        int movX;
        int movY;
        float scale;
        SDL_Rect spriteRect;
        SDL_Surface* sprite;

        Sprite(int _posX, int _posY, int _movX, int _movY, float _scale, SDL_Surface* _sprite) {
        posX = _posX;
        posY = _posY;
        movX = _movX;
        movY = _movY;
        scale = _scale;
        sprite = _sprite;

        actualizarRect();
    }

    void mover() {
        posX += movX;
        posY += movY;
        actualizarRect();
    }

    //-----------------------------------------------------------------------------
    private:
        void actualizarRect() {
            int rectX = static_cast<int>(posX-sprite->w/2 * scale);
            int rectY = static_cast<int>(posY-sprite->h/2* scale);
            int rectW = static_cast<int>(sprite->w* scale);
            int rectH = static_cast<int>(sprite->h* scale);
            spriteRect = { rectX, rectY, rectW, rectH };
        }
};