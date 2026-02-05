#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "Sprite.cpp"
#include "random.cpp"
#include <iostream>


static SDL_Window* win;
static SDL_Surface* winSurface;
static Mix_Chunk* sound;

//Inicializa SDL, crea la ventana y la superficie
void inicializarSDL() {
	//Inicializar SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	//Crear ventana
	win = SDL_CreateWindow("Demo 1", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
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

	//Inicializar audio
	if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0) {
        std::cerr << "Error al inicializar SDL_mixer: " << Mix_GetError() << std::endl;
        exit(1);
    }
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cerr << "Error al abrir el dispositivo de audio: " << Mix_GetError() << std::endl;
        exit(1);
    }
}

//Cierra la ventana y termina SDL
void cerrarSDL() {
	Mix_FreeChunk(sound);			//Liberar audio
	Mix_CloseAudio();
	SDL_FreeSurface(winSurface);	//Liberar superficie
	winSurface = NULL;
	SDL_DestroyWindow(win);		//Cerrar ventana
	SDL_Quit();					//Cerrar SDL
}

void gameLoop(Sprite& sprite, int w_width, int w_height) {
	SDL_Event event;

	while (true) {
		//Comprobar entradas
		while (SDL_PollEvent(&event)) {
			//Cerrar ventana o pulsar escape -> terminar
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
				cerrarSDL();
			}
			//Cambiar posición y movimiento con Enter
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				float newPosX = getRandomfloat(sprite.spriteRect.w/2, w_width-sprite.spriteRect.w/2);
				float newPosY = getRandomfloat(sprite.spriteRect.h/2, w_height-sprite.spriteRect.h/2);
				float newMovX = getRandomfloat(-1, 1);
				float newMovY = getRandomfloat(-1, 1);
				float newSpeed = getRandomfloat(0.03, 0.3);
				sprite.setPosX(newPosX);
				sprite.setPosY(newPosY);
				sprite.setMovX(newMovX);
				sprite.setMovY(newMovY);
				sprite.speed = newSpeed;
			}
		}

		//Actualizar movimiento de sprites
		sprite.mover();
		
		//Rebotar con bordes
		if (sprite.spriteRect.x <= 0 || sprite.spriteRect.x+sprite.spriteRect.w >= w_width) {
			Mix_PlayChannel(-1,sound, 0);
            sprite.setMovX(sprite.getMovX()*-1);
			
        }
        else if (sprite.spriteRect.y <= 0 || sprite.spriteRect.y+sprite.spriteRect.h >= w_height) {
			Mix_PlayChannel(-1,sound, 0);
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

	//Cargar audio
	sound = Mix_LoadWAV("assets/sonido.mp3");
    if (!sound) {
        std::cout << "Error loading music: " << Mix_GetError() << std::endl;
		exit(-1);
    }
	
	//Game loop
	gameLoop(sprite, w_width, w_height);

	cerrarSDL();
	return 0;
}