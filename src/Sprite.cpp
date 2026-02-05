#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>

class Sprite {
    private:
        float posX;
        float posY;
        float scale;
        float movX;
        float movY;
    public:
        float speed;
        SDL_Rect spriteRect;
        SDL_Surface* sprite;

        Sprite(int _posX, int _posY, float _scale, SDL_Surface* _sprite) {
        posX = _posX;
        posY = _posY;
        movX = 0;
        movY = 0;
        speed = 0.0f;
        scale = _scale;
        sprite = _sprite;

        actualizarRect();
    }

    void setMovimiento(float _movX, float _movY, float _speed) {
        //Normalizar el vector de movimiento
        float modulo = std::sqrt(_movX * _movX + _movY * _movY);

        movX = _movX / modulo;
        movY = _movY / modulo;
        speed = _speed;
    }

    void mover() {
        posX += movX * speed;
        posY += movY * speed;
        actualizarRect();
    }

    float getMovX() {
        return movX;
    }
    float getMovY() {
        return movY;
    }
    void setMovX(float mov) {
        movX = mov;
        actualizarRect();
    }
    void setMovY(float mov) {
        movY = mov;
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