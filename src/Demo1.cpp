#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "Sprite.cpp"
#include "random.cpp"
#include "tiempo.cpp"
#include <iostream>
#include <windows.h>
#include "ventana.cpp"
#include <cmath>


Mix_Chunk* sound;


//Inicializa SDL, crea la ventana y la superficie
void inicializarSistema() {
	//Inicializar SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	ventana::inicializar();

	//Inicializar audio
	if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0) {
        std::cerr << "Error al inicializar SDL_mixer: " << Mix_GetError() << std::endl;
        exit(1);
    }
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cerr << "Error al abrir el dispositivo de audio: " << Mix_GetError() << std::endl;
        exit(1);
    }

	//Inicializar tiempo
	tiempo::inicializarDeltaTime();

	//Forzar abrir consola
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
}

//Cierra la ventana y termina SDL
void cerrarSDL() {
	Mix_FreeChunk(sound);	//Liberar audio
	Mix_CloseAudio();
	ventana::cerrar();		//Cerrar ventana
	SDL_Quit();				//Cerrar SDL
}

void gameLoop(Sprite& sprite) {
	SDL_Event event;

	while (true) {
		tiempo::setDeltaTime();

		//Comprobar entradas
		if (SDL_PollEvent(&event)) {
			//Cerrar ventana o pulsar escape -> terminar
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
				cerrarSDL();
			}
			//Redimensionado de la ventana
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
				float xProportion = sprite.getPosX() / (float) ventana::width;
				float yProportion = sprite.getPosY() / (float) ventana::height;

				ventana::actualizarDimensiones();
				sprite.setPos(ventana::width * xProportion, ventana::height * yProportion);
			}
			//Cambiar posición y movimiento con Enter
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				float newPosX = getRandomFloat(sprite.spriteRect.w/2, ventana::width-sprite.spriteRect.w/2);
				float newPosY = getRandomFloat(sprite.spriteRect.h/2, ventana::height-sprite.spriteRect.h/2);
				float newMovX = getRandomFloat(-1, 1);
				float newMovY = getRandomFloat(-1, 1);
				float newSpeed = getRandomFloat(50, 700);
				sprite.setPosX(newPosX);
				sprite.setPosY(newPosY);
				sprite.setMovX(newMovX);
				sprite.setMovY(newMovY);
				sprite.speed = newSpeed;
			}
			//Escalar sprite con flecha arriba y flecha abajo SDLK_UP
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP) {
				if (sprite.getScale() < 2) {
					sprite.setScale(sprite.getScale()+0.1);

					//Corregir colisiones
					while (sprite.spriteRect.x <= 0) {
						sprite.setPosX(sprite.getPosX()+1);
					}
					while (sprite.spriteRect.x+sprite.spriteRect.w >= ventana::width) {
						sprite.setPosX(sprite.getPosX()-1);
					}
					while(sprite.spriteRect.y <= 0) {
						sprite.setPosY(sprite.getPosY()+1);
					}
					while (sprite.spriteRect.y+sprite.spriteRect.h >= ventana::height) {
						sprite.setPosY(sprite.getPosY()-1);
					}
				}
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN) {
				if (sprite.getScale() > 0.3) {
					sprite.setScale(sprite.getScale()-0.1);
				}
			}
		}

		//Actualizar movimiento de sprites
		sprite.mover();
		
		//Rebotar con bordes
		if (sprite.spriteRect.x <= 0) {				//Borde izquierdo
			Mix_PlayChannel(-1,sound, 0);
            sprite.setMovX(fabs(sprite.getMovX()));
        }
        else if (sprite.spriteRect.y <= 0) {		//Borde arriba
			Mix_PlayChannel(-1,sound, 0);
            sprite.setMovY(fabs(sprite.getMovY()));
        }
		else if (sprite.spriteRect.x+sprite.spriteRect.w >= ventana::width) {	//Borde derecho
			Mix_PlayChannel(-1,sound, 0);
            sprite.setMovX(-fabs(sprite.getMovX()));
		}
		else if (sprite.spriteRect.y+sprite.spriteRect.h >= ventana::height) {	//Borde abajo
			Mix_PlayChannel(-1,sound, 0);
			sprite.setMovY(-fabs(sprite.getMovY()));
		}

		//Repintar sprite y actualizar superficie de la ventana
		SDL_FillRect(ventana::winSurface, NULL, SDL_MapRGB(ventana::winSurface->format, 0, 0, 0));
		SDL_BlitScaled(sprite.sprite, NULL, ventana::winSurface, &sprite.spriteRect);
		SDL_UpdateWindowSurface(ventana::win);
	}
}

int main(int argc, char* argv[]) {
	//Inicializar
	inicializarSistema();

	//Poner sprite en el centro de la ventana
	Sprite sprite = Sprite(ventana::width/2, ventana::height/2, 0.5, IMG_Load("assets/patata.jpg"));
	sprite.setMovimiento(1.0f, 1.0f, 300);

	//Cargar audio
	sound = Mix_LoadWAV("assets/sonido.mp3");
    if (!sound) {
        std::cout << "Error loading music: " << Mix_GetError() << std::endl;
		exit(-1);
    }
	
	//Game loop
	gameLoop(sprite);

	cerrarSDL();
	return 0;
}