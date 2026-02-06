#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "Sprite.cpp"
#include "random.cpp"
#include "tiempo.cpp"
#include <iostream>
#include <windows.h>
#include "ventana.cpp"


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

//Pinta todos los objetos en la ventana
void pintar(Sprite objetos[], int nObjetos) {
	SDL_FillRect(ventana::winSurface, NULL, SDL_MapRGB(ventana::winSurface->format, 0, 0, 0));
	for (int i = 0; i < nObjetos; i++) {
		SDL_BlitScaled(objetos[i].sprite, NULL, ventana::winSurface, &objetos[i].spriteRect);
	}
	SDL_UpdateWindowSurface(ventana::win);
}

void gameLoop(Sprite objetos[], int nObjetos) {
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
				//Coger posicion relativa a la ventana
				float proportions[nObjetos][2];
				for (int i = 0; i < nObjetos; i++) {
					proportions[i][0] = objetos[i].getPosX() / (float) ventana::width;
					proportions[i][1] = objetos[i].getPosY() / (float) ventana::height;
				}

				//Actualizar dimensiones
				ventana::actualizarDimensiones();

				//Reajustar posiciones
				for (int i = 0; i < nObjetos; i++) {
					objetos[i].setPos(ventana::width * proportions[i][0], ventana::height * proportions[i][1]);
				}
			}
			//Cambiar posición y movimiento con Enter
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				for (int i = 0; i < nObjetos; i++) {
					float newPosX = getRandomFloat(objetos[i].spriteRect.w/2, ventana::width-objetos[i].spriteRect.w/2);
					float newPosY = getRandomFloat(objetos[i].spriteRect.h/2, ventana::height-objetos[i].spriteRect.h/2);
					float newMovX = getRandomFloat(-1, 1);
					float newMovY = getRandomFloat(-1, 1);
					float newSpeed = getRandomFloat(50, 700);
					objetos[i].setPosX(newPosX);
					objetos[i].setPosY(newPosY);
					objetos[i].setMovX(newMovX);
					objetos[i].setMovY(newMovY);
					objetos[i].speed = newSpeed;
				}
			}
			//Escalar sprite con flecha arriba y flecha abajo SDLK_UP
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP) {
				for (int i = 0; i < nObjetos; i++) {
					if (objetos[i].getScale() < 2) {
						objetos[i].setScale(objetos[i].getScale()+0.1);

						//Corregir colisiones
						while (objetos[i].spriteRect.x <= 0) {
							objetos[i].setPosX(objetos[i].getPosX()+1);
						}
						while (objetos[i].spriteRect.x+objetos[i].spriteRect.w >= ventana::width) {
							objetos[i].setPosX(objetos[i].getPosX()-1);
						}
						while(objetos[i].spriteRect.y <= 0) {
							objetos[i].setPosY(objetos[i].getPosY()+1);
						}
						while (objetos[i].spriteRect.y+objetos[i].spriteRect.h >= ventana::height) {
							objetos[i].setPosY(objetos[i].getPosY()-1);
						}
					}
				}
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN) {
				for (int i = 0; i < nObjetos; i++) {
					if (objetos[i].getScale() > 0.3) {
						objetos[i].setScale(objetos[i].getScale()-0.1);
					}
				}
			}
		}

		//Actualizar movimiento de sprites
		for (int i = 0; i < nObjetos; i++) {
			objetos[i].mover();
		}
		
		//Rebotar con bordes
		for (int i = 0; i < nObjetos; i++) {
			if (objetos[i].spriteRect.x <= 0) {				//Borde izquierdo
				Mix_PlayChannel(-1,sound, 0);
				objetos[i].setMovX(abs(objetos[i].getMovX()));
			}
			else if (objetos[i].spriteRect.y <= 0) {		//Borde arriba
				Mix_PlayChannel(-1,sound, 0);
				objetos[i].setMovY(abs(objetos[i].getMovY()));
			}
			else if (objetos[i].spriteRect.x+objetos[i].spriteRect.w >= ventana::width) {	//Borde derecho
				Mix_PlayChannel(-1,sound, 0);
				objetos[i].setMovX(-abs(objetos[i].getMovX()));
			}
			else if (objetos[i].spriteRect.y+objetos[i].spriteRect.h >= ventana::height) {	//Borde abajo
				Mix_PlayChannel(-1,sound, 0);
				objetos[i].setMovY(-abs(objetos[i].getMovY()));
			}
		}

		//Repintar objetos
		pintar(objetos, nObjetos);
	}
}

int main(int argc, char* argv[]) {
	//Inicializar
	inicializarSistema();

	//Crear objetos
	int nObjetos = 5;
	Sprite objetos[5];
	SDL_Surface* imagen = IMG_Load("assets/patata.jpg");	//Todos los sprites van a tener la misma imagen en memoria
	for (int i = 0; i < nObjetos; i++) {
		float posX = getRandomFloat(0+imagen->w/2, ventana::width-imagen->w/2);
		float posY = getRandomFloat(0+imagen->h/2, ventana::height-imagen->h/2);
		float scale = getRandomFloat(0.3, 1);
		float movX = getRandomFloat(-1, 1);
		float movY = getRandomFloat(-1, 1);
		float speed = getRandomFloat(50, 700);

		objetos[i] = Sprite(posX, posY, scale, imagen);
		objetos[i].setMovimiento(movX, movY, speed);
	}
	//Cargar audio
	sound = Mix_LoadWAV("assets/sonido.mp3");
    if (!sound) {
        std::cout << "Error loading music: " << Mix_GetError() << std::endl;
		exit(-1);
    }
	
	//Game loop
	gameLoop(objetos, nObjetos);

	cerrarSDL();
	return 0;
}