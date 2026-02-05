#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Sprite.cpp"
#include <iostream>
#include <random>


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

void gameLoop(Sprite& sprite, int w_width, int w_height) {
	for (int i = 0; i <= 10000; i++) {
		sprite.mover();
		
		//Rebotar con bordes
		if (sprite.spriteRect.x <= 0 || sprite.spriteRect.x+sprite.spriteRect.w >= w_width) {
            sprite.setMovX(sprite.getMovX()*-1);
        }
        else if (sprite.spriteRect.y <= 0 || sprite.spriteRect.y+sprite.spriteRect.h >= w_height) {
            sprite.setMovY(sprite.getMovY()*-1);
        }

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
	Sprite sprite = Sprite(w_width/2, w_height/2, 0.5, IMG_Load("assets/patata.jpg"));
	sprite.setMovimiento(1.0f, 1.0f, 0.1);
	
	//Mover sprite
	gameLoop(sprite, w_width, w_height);

	//SDL_Delay(3000); //Esperar 3 segundos para cerrar la ventana

	cerrarSDL();
	return 0;
}