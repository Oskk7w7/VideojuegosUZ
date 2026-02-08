#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_thread.h>
#include "Sprite.cpp"
#include "random.cpp"
#include "tiempo.cpp"
#include <iostream>
#include <windows.h>
#include "ventana.cpp"
#include <cmath>
#include <vector>
#include <algorithm>


Mix_Chunk* sound;

// Variables globales para multithreading
SDL_mutex* mutexSprites = nullptr;
bool corriendo = true;
int nObjetos = 5;
Sprite* objetos = nullptr;


//Inicializa SDL, crea la ventana y la superficie
void inicializarSistema() {
	//Inicializar SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	ventana::inicializar();
	
	//Inicializar mutex para sincronización
	mutexSprites = SDL_CreateMutex();
	if (mutexSprites == nullptr) {
		std::cerr << "Error al crear mutex: " << SDL_GetError() << std::endl;
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

	//Inicializar tiempo
	tiempo::inicializarDeltaTime();

	//Forzar abrir consola
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
}

//Cierra la ventana y termina SDL
void cerrarSDL() {
	corriendo = false;
	if (mutexSprites != nullptr) {
		SDL_DestroyMutex(mutexSprites);
	}
	Mix_FreeChunk(sound);	//Liberar audio
	Mix_CloseAudio();
	ventana::cerrar();		//Cerrar ventana
	SDL_Quit();				//Cerrar SDL
}

// Función para detectar colisión entre dos sprites
bool hayColision(const Sprite& a, const Sprite& b) {
	SDL_Rect rectA = a.spriteRect;
	SDL_Rect rectB = b.spriteRect;
	
	// Verificar si hay solapamiento
	return !(rectA.x + rectA.w < rectB.x || 
	         rectB.x + rectB.w < rectA.x ||
	         rectA.y + rectA.h < rectB.y ||
	         rectB.y + rectB.h < rectA.y);
}

// Función para resolver colisión elástica entre dos sprites
void resolverColisionElastica(Sprite& a, Sprite& b) {
	// Guardar speeds originales para mantenerlas
	float speedAOriginal = a.speed;
	float speedBOriginal = b.speed;
	
	// Calcular vector de separación
	float dx = b.getPosX() - a.getPosX();
	float dy = b.getPosY() - a.getPosY();
	float distancia = std::sqrt(dx * dx + dy * dy);
	
	if (distancia < 0.001f) return; // Evitar división por cero
	
	// Normalizar
	dx /= distancia;
	dy /= distancia;
	
	// Calcular velocidades relativas
	float vRelX = (b.getMovX() * b.speed) - (a.getMovX() * a.speed);
	float vRelY = (b.getMovY() * b.speed) - (a.getMovY() * a.speed);
	
	// Velocidad relativa en la dirección de la colisión
	float velocidadRelativa = vRelX * dx + vRelY * dy;
	
	// No hacer nada si los objetos se están separando
	if (velocidadRelativa > 0) return;
	
	// Coeficiente de restitución (elasticidad)
	float restitucion = 0.8f;
	
	// Calcular impulso
	float impulso = -(1 + restitucion) * velocidadRelativa;
	
	// Aplicar impulso (asumiendo masas iguales)
	float cambioVelX = impulso * dx;
	float cambioVelY = impulso * dy;
	
	// Calcular nuevas velocidades vectoriales
	float nuevaVelAX = a.getMovX() * a.speed - cambioVelX;
	float nuevaVelAY = a.getMovY() * a.speed - cambioVelY;
	float nuevaVelBX = b.getMovX() * b.speed + cambioVelX;
	float nuevaVelBY = b.getMovY() * b.speed + cambioVelY;
	
	// Normalizar las nuevas direcciones pero mantener las speeds originales
	float moduloA = std::sqrt(nuevaVelAX * nuevaVelAX + nuevaVelAY * nuevaVelAY);
	float moduloB = std::sqrt(nuevaVelBX * nuevaVelBX + nuevaVelBY * nuevaVelBY);
	
	// Actualizar solo las direcciones, manteniendo las speeds originales
	if (moduloA > 0.001f) {
		a.setMovimiento(nuevaVelAX / moduloA, nuevaVelAY / moduloA, speedAOriginal);
	}
	if (moduloB > 0.001f) {
		b.setMovimiento(nuevaVelBX / moduloB, nuevaVelBY / moduloB, speedBOriginal);
	}
	
	// Separar los sprites para evitar que se queden pegados
	float separacion = (a.spriteRect.w + b.spriteRect.w) / 2.0f - distancia;
	if (separacion > 0) {
		float separacionX = dx * separacion * 0.5f;
		float separacionY = dy * separacion * 0.5f;
		a.setPos(a.getPosX() - separacionX, a.getPosY() - separacionY);
		b.setPos(b.getPosX() + separacionX, b.getPosY() + separacionY);
	}
}

//Pinta todos los objetos en la ventana ordenados por profundidad
void pintar() {
	// Copiar datos necesarios con el mutex bloqueado (rápido)
	std::vector<std::pair<int, SDL_Rect>> datosRenderizado;
	std::vector<SDL_Surface*> sprites;
	
	SDL_LockMutex(mutexSprites);
	
	// Crear vector de índices ordenados por z-index (menor z-index primero = más atrás)
	std::vector<std::pair<int, int>> indicesOrdenados;
	for (int i = 0; i < nObjetos; i++) {
		indicesOrdenados.push_back({objetos[i].getZIndex(), i});
	}
	
	// Ordenar por z-index (menor = más atrás, se dibuja primero)
	std::sort(indicesOrdenados.begin(), indicesOrdenados.end(),
		[](const std::pair<int, int>& a, const std::pair<int, int>& b) {
			return a.first < b.first;
		});
	
	// Copiar los rectángulos y sprites necesarios
	for (auto& par : indicesOrdenados) {
		int i = par.second;
		datosRenderizado.push_back({i, objetos[i].spriteRect});
		sprites.push_back(objetos[i].sprite);
	}
	
	SDL_UnlockMutex(mutexSprites);
	
	// Renderizar sin el mutex bloqueado
	SDL_FillRect(ventana::winSurface, NULL, SDL_MapRGB(ventana::winSurface->format, 0, 0, 0));
	
	// Dibujar en orden de profundidad
	for (size_t idx = 0; idx < datosRenderizado.size(); idx++) {
		SDL_BlitScaled(sprites[idx], NULL, ventana::winSurface, &datosRenderizado[idx].second);
	}
	
	SDL_UpdateWindowSurface(ventana::win);
}

// Función del thread para actualizar sprites
int actualizarSpritesThread(void* data) {
	Uint32 ultimoTiempo = SDL_GetTicks();
	const Uint32 intervaloActualizacion = 16; // ~60 FPS (1000ms / 60 = 16.67ms)
	
	while (corriendo) {
		Uint32 tiempoActual = SDL_GetTicks();
		Uint32 deltaTiempo = tiempoActual - ultimoTiempo;
		
		// Limitar velocidad de actualización
		if (deltaTiempo < intervaloActualizacion) {
			SDL_Delay(intervaloActualizacion - deltaTiempo);
			continue;
		}
		
		ultimoTiempo = SDL_GetTicks();
		
		SDL_LockMutex(mutexSprites);
		
		// Actualizar movimiento de sprites
		for (int i = 0; i < nObjetos; i++) {
			objetos[i].mover();
		}
		
		// Rebotar con bordes
		for (int i = 0; i < nObjetos; i++) {
			if (objetos[i].spriteRect.x <= 0) {				//Borde izquierdo
				Mix_PlayChannel(-1, sound, 0);
				objetos[i].setMovX(fabs(objetos[i].getMovX()));
			}
			else if (objetos[i].spriteRect.y <= 0) {		//Borde arriba
				Mix_PlayChannel(-1, sound, 0);
				objetos[i].setMovY(fabs(objetos[i].getMovY()));
			}
			else if (objetos[i].spriteRect.x+objetos[i].spriteRect.w >= ventana::width) {	//Borde derecho
				Mix_PlayChannel(-1, sound, 0);
				objetos[i].setMovX(-fabs(objetos[i].getMovX()));
			}
			else if (objetos[i].spriteRect.y+objetos[i].spriteRect.h >= ventana::height) {	//Borde abajo
				Mix_PlayChannel(-1, sound, 0);
				objetos[i].setMovY(-fabs(objetos[i].getMovY()));
			}
		}
		
		// Detectar y resolver colisiones entre sprites (solo en el mismo plano)
		for (int i = 0; i < nObjetos; i++) {
			for (int j = i + 1; j < nObjetos; j++) {
				// Solo colisionar si están en el mismo plano (según enunciado)
				if (objetos[i].getZIndex() == objetos[j].getZIndex()) {
					if (hayColision(objetos[i], objetos[j])) {
						resolverColisionElastica(objetos[i], objetos[j]);
						Mix_PlayChannel(-1, sound, 0);
					}
				}
			}
		}
		
		SDL_UnlockMutex(mutexSprites);
	}
	return 0;
}

void gameLoop() {
	SDL_Event event;
	
	// Crear thread para actualizar sprites
	SDL_Thread* threadActualizacion = SDL_CreateThread(actualizarSpritesThread, "ActualizarSprites", nullptr);
	if (threadActualizacion == nullptr) {
		std::cerr << "Error al crear thread: " << SDL_GetError() << std::endl;
		return;
	}

	while (corriendo) {
		tiempo::setDeltaTime();

		//Comprobar entradas
		if (SDL_PollEvent(&event)) {
			//Cerrar ventana o pulsar escape -> terminar
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
				cerrarSDL();
			}
			//Redimensionado de la ventana
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
				SDL_LockMutex(mutexSprites);
				
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
				
				SDL_UnlockMutex(mutexSprites);
			}
			//Cambiar posición y movimiento con Enter
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				SDL_LockMutex(mutexSprites);
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
				SDL_UnlockMutex(mutexSprites);
			}
			//Escalar sprite con flecha arriba y flecha abajo SDLK_UP
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP) {
				SDL_LockMutex(mutexSprites);
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
				SDL_UnlockMutex(mutexSprites);
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN) {
				SDL_LockMutex(mutexSprites);
				for (int i = 0; i < nObjetos; i++) {
					if (objetos[i].getScale() > 0.3) {
						objetos[i].setScale(objetos[i].getScale()-0.1);
					}
				}
				SDL_UnlockMutex(mutexSprites);
			}
			// Cambiar plano con teclas 1-5
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_5) {
				int indice = event.key.keysym.sym - SDLK_1;
				if (indice < nObjetos) {
					SDL_LockMutex(mutexSprites);
					// Cambiar entre 3 planos (0, 1, 2)
					int nuevoPlano = (objetos[indice].getZIndex() + 1) % 3;
					objetos[indice].setZIndex(nuevoPlano);
					SDL_UnlockMutex(mutexSprites);
				}
			}
		}

		//Repintar objetos (el thread se encarga de actualizar)
		pintar();
		
		// Control de FPS del renderizado (~60 FPS)
		SDL_Delay(16);
	}
	
	// Esperar a que termine el thread
	SDL_WaitThread(threadActualizacion, nullptr);
}

int main(int argc, char* argv[]) {
	//Inicializar
	inicializarSistema();

	//Crear objetos
	nObjetos = 5;
	objetos = new Sprite[nObjetos];
	SDL_Surface* imagen = IMG_Load("assets/patata.jpg");	//Todos los sprites van a tener la misma imagen en memoria
	for (int i = 0; i < nObjetos; i++) {
		float posX = getRandomFloat(0+imagen->w/2, ventana::width-imagen->w/2);
		float posY = getRandomFloat(0+imagen->h/2, ventana::height-imagen->h/2);
		float scale = getRandomFloat(0.3, 1);
		float movX = getRandomFloat(-1, 1);
		float movY = getRandomFloat(-1, 1);
		float speed = getRandomFloat(50, 700);
		
		// Asignar planos aleatorios (0, 1, o 2)
		int plano = getRandomInt(0, 2);

		objetos[i] = Sprite(posX, posY, scale, imagen);
		objetos[i].setMovimiento(movX, movY, speed);
		objetos[i].setZIndex(plano);
	}
	//Cargar audio
	sound = Mix_LoadWAV("assets/sonido.mp3");
    if (!sound) {
        std::cout << "Error loading music: " << Mix_GetError() << std::endl;
		exit(-1);
    }
	
	//Game loop
	gameLoop();

	cerrarSDL();
	delete[] objetos;
	return 0;
}