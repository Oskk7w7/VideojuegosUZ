#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Sprite.cpp"
#include <iostream>


static SDL_Window* win;
static SDL_Surface* winSurface;

//Inicializa SDL, crea la ventana y la superficie
void inicializarSDL() {
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
	winSurface = SDL_GetWindowSurface(win);
	if (winSurface == nullptr) {
		std::cerr << "SDL_GetWindowSurface Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
}

//Cierra la ventana y termina SDL
void cerrarSDL() {
	SDL_FreeSurface(winSurface);	//Liberar superficie
	winSurface = NULL;
	SDL_DestroyWindow(win);		//Cerrar ventana
	SDL_Quit();					//Cerrar SDL
}

void gameLoop(Sprite& sprite) {
	for (int i = 0; i <= 1000; i++) {
		sprite.mover();

		//Repintar sprite y actualizar superficie de la ventana
		SDL_FillRect(winSurface, NULL, SDL_MapRGB(winSurface->format, 0, 0, 0));
		SDL_BlitScaled(sprite.sprite, NULL, winSurface, &sprite.spriteRect);
		SDL_UpdateWindowSurface(win);
	}
}

int main(int argc, char* argv[]) {
	//Inicializar
	inicializarSDL();


	//Dimensiones de la ventana
	int w_width = 0;
	int w_height = 0;
	SDL_GetWindowSize(win, &w_width, &w_height);

	//Poner sprite en el centro de la ventana
	Sprite sprite = Sprite(w_width/2, w_height/2, 1, 1, 0.5, IMG_Load("assets/patata.jpg"));
	
	//Mover sprite
	gameLoop(sprite);

	//SDL_Delay(3000); //Esperar 3 segundos para cerrar la ventana

	cerrarSDL();
	return 0;
}