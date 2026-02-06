#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>
#include "tiempo.cpp"
#include "ventana.cpp"

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

        Sprite() {
            posX = 0;
            posY = 0;
            movX = 0;
            movY = 0;
            speed = 0.0f;
            scale = 1;
            sprite = NULL;
            spriteRect = {0,0,1,1};
        }

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
        posX += movX * speed * tiempo::DELTA_TIME;
        posY += movY * speed * tiempo::DELTA_TIME;
        actualizarRect();
    }

    //--------------------
    float getPosX() {
        return posX;
    }
    float getPosY() {
        return posY;
    }
    void setPosX(float pos) {
        posX = pos;
        actualizarRect();
    }
    void setPosY(float pos) {
        posY = pos;
        actualizarRect();
    }
    void setPos(float x, float y) {
        posX = x;
        posY = y;
        actualizarRect();
    }
    float getMovX() {
        return movX;
    }
    float getMovY() {
        return movY;
    }
    void setMovX(float mov) {
        setMovimiento(mov, movY, speed);
    }
    void setMovY(float mov) {
        setMovimiento(movX, mov, speed);
    }
    float getScale() {
        return scale;
    }
    void setScale(float sc) {
        scale = sc;
        actualizarRect();
    }
    //--------------------

    //-----------------------------------------------------------------------------
    private:
        void actualizarRect() {
            int rectX = static_cast<int>(posX-sprite->w/2 * scale * ventana::width / ventana::initWidth);
            int rectY = static_cast<int>(posY-sprite->h/2* scale * ventana::height / ventana::initHeight);
            int rectW = static_cast<int>(sprite->w* scale * ventana::width / 640);
            int rectH = static_cast<int>(sprite->h* scale * ventana::height / 480);
            spriteRect = { rectX, rectY, rectW, rectH };
        }
};