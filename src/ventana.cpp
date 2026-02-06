#pragma once
#include <SDL2/SDL.h>
#include <iostream>

namespace ventana {
    //Dimensiones iniciales de la ventana
    int initWidth = 640;
    int initHeight = 480;

    //Dimensiones actuales de la ventana
    int width = 0;
    int height = 0;

    //Ventana y superficie
    SDL_Window* win;
    SDL_Surface* winSurface;


    void inicializar() {
        //Crear ventana
        win = SDL_CreateWindow("Demo 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, initWidth, initHeight, SDL_WINDOW_RESIZABLE);
        if (win == nullptr) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }

        //Obtener dimensiones de la ventana
        SDL_GetWindowSize(win, &width, &height);

        //Crear superficie
        winSurface = SDL_GetWindowSurface(win);
        if (winSurface == nullptr) {
            std::cerr << "SDL_GetWindowSurface Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }
    }

    void actualizarDimensiones() {
        winSurface = SDL_GetWindowSurface(win);
		SDL_GetWindowSize(win, &width, &height);
    }

    void cerrar() {
        SDL_FreeSurface(winSurface);	//Liberar superficie
	    winSurface = NULL;
	    SDL_DestroyWindow(win);		//Cerrar ventana
    }
}