#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
	//Inicializar SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	//Crear ventana
	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Delay(3000); //Esperar 3 segundos para cerrar la ventana

	//Cerrar ventana
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}