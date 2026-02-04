#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

//Inicializa SDL, crea la ventana y la superficie
void inicializarSDL(SDL_Window *win, SDL_Surface *surface) {
	//Inicializar SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	//Crear ventana
	win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	//Crear superficie
	surface = SDL_GetWindowSurface(win);
	if (surface == nullptr) {
		std::cerr << "SDL_GetWindowSurface Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
}

//Cierra la ventana y termina SDL
void cerrarSDL(SDL_Window *win, SDL_Surface *surface) {
	SDL_FreeSurface(surface);	//Liberar superficie
	surface = NULL;
	SDL_DestroyWindow(win);		//Cerrar ventana
	SDL_Quit();					//Cerrar SDL
}

int main(int argc, char* argv[]) {
	//Inicializar
	SDL_Window* win;
	SDL_Surface* surface;
	inicializarSDL(win, surface);



	SDL_UpdateWindowSurface(win);


	SDL_Delay(3000); //Esperar 3 segundos para cerrar la ventana

	cerrarSDL(win, surface);
	return 0;
}