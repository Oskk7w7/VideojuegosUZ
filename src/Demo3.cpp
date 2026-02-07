#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "random.cpp"
#include "tiempo.cpp"
#include "ventana.cpp"
#include <iostream>
#include <windows.h>
#include <cmath>
#include <vector>
#include <algorithm>

// Estructura para una estrella en el campo 3D
struct Estrella {
    float x, y, z;          // Posición 3D
    float rotacion;         // Ángulo de rotación
    float velocidadRotacion; // Velocidad de rotación
    float velocidadZ;       // Velocidad de movimiento hacia la cámara
    SDL_Texture* textura;   // Textura de la estrella
    int anchoOriginal, altoOriginal;
    
    Estrella() : x(0), y(0), z(1.0f), rotacion(0), velocidadRotacion(0), velocidadZ(0), textura(nullptr), anchoOriginal(0), altoOriginal(0) {}
};

// Variables globales
SDL_Renderer* renderer = nullptr;
SDL_Texture* texturaEstrella = nullptr;
std::vector<Estrella> estrellas;
const int NUM_ESTRELLAS = 200;
float velocidadBase = 50.0f;
float velocidadHiperespacio = 300.0f;
bool hiperespacio = false;
int anchoOriginal = 0;
int altoOriginal = 0;

// Inicializa SDL, crea la ventana y el renderer
void inicializarSistema() {
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Crear ventana directamente (sin superficie, porque usamos renderer)
    ventana::win = SDL_CreateWindow("Demo 3 - Campo de Estrellas", 
                                     SDL_WINDOWPOS_UNDEFINED, 
                                     SDL_WINDOWPOS_UNDEFINED, 
                                     ventana::initWidth, 
                                     ventana::initHeight, 
                                     SDL_WINDOW_RESIZABLE);
    if (ventana::win == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    // Obtener dimensiones de la ventana
    SDL_GetWindowSize(ventana::win, &ventana::width, &ventana::height);

    // Crear renderer (necesario para rotación)
    renderer = SDL_CreateRenderer(ventana::win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(ventana::win);
        SDL_Quit();
        exit(1);
    }

    // Inicializar SDL_image
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        exit(1);
    }

    // Cargar imagen de la estrella
    SDL_Surface* superficie = IMG_Load("assets/patata.jpg");
    if (superficie == nullptr) {
        std::cerr << "No se pudo cargar la imagen: " << IMG_GetError() << std::endl;
        exit(1);
    }
    
    texturaEstrella = SDL_CreateTextureFromSurface(renderer, superficie);
    if (texturaEstrella == nullptr) {
        std::cerr << "No se pudo crear la textura: " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    anchoOriginal = superficie->w;
    altoOriginal = superficie->h;
    SDL_FreeSurface(superficie);

    // Inicializar tiempo
    tiempo::inicializarDeltaTime();

    // Forzar abrir consola
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
}

// Cierra la ventana y termina SDL
void cerrarSDL() {
    SDL_DestroyTexture(texturaEstrella);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana::win);
    SDL_Quit();
}

// Reinicia una estrella al fondo
void reiniciarEstrella(Estrella& estrella) {
    estrella.x = getRandomFloat(-ventana::width * 0.5f, ventana::width * 0.5f);
    estrella.y = getRandomFloat(-ventana::height * 0.5f, ventana::height * 0.5f);
    estrella.z = getRandomFloat(500.0f, 2000.0f);
    estrella.rotacion = getRandomFloat(0.0f, 360.0f);
    estrella.velocidadRotacion = getRandomFloat(30.0f, 120.0f);
    estrella.velocidadZ = getRandomFloat(velocidadBase * 0.5f, velocidadBase * 1.5f);
}

// Inicializa todas las estrellas
void inicializarEstrellas() {
    estrellas.clear();
    estrellas.resize(NUM_ESTRELLAS);
    
    for (int i = 0; i < NUM_ESTRELLAS; i++) {
        reiniciarEstrella(estrellas[i]);
        estrellas[i].textura = texturaEstrella;
    }
}

// Actualiza las estrellas
void actualizarEstrellas() {
    float velocidadActual = hiperespacio ? velocidadHiperespacio : velocidadBase;
    
    for (auto& estrella : estrellas) {
        // Actualizar rotación
        estrella.rotacion += estrella.velocidadRotacion * tiempo::DELTA_TIME;
        if (estrella.rotacion >= 360.0f) {
            estrella.rotacion -= 360.0f;
        }
        
        // Mover hacia la cámara (zoom)
        estrella.z -= estrella.velocidadZ * tiempo::DELTA_TIME * (velocidadActual / velocidadBase);
        
        // Si la estrella pasa la cámara, reiniciarla al fondo
        if (estrella.z <= 0.1f) {
            reiniciarEstrella(estrella);
        }
    }
}

// Dibuja todas las estrellas
void dibujarEstrellas() {
    // Limpiar pantalla (negro)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Dibujar estrellas ordenadas por profundidad (las más lejanas primero)
    std::vector<std::pair<float, Estrella*>> estrellasOrdenadas;
    for (auto& estrella : estrellas) {
        estrellasOrdenadas.push_back({estrella.z, &estrella});
    }
    
    // Ordenar por profundidad (z mayor = más lejano)
    std::sort(estrellasOrdenadas.begin(), estrellasOrdenadas.end(),
              [](const std::pair<float, Estrella*>& a, const std::pair<float, Estrella*>& b) {
                  return a.first > b.first;
              });
    
    // Centro de la pantalla
    float centroX = ventana::width / 2.0f;
    float centroY = ventana::height / 2.0f;
    
    // Dibujar cada estrella
    for (auto& par : estrellasOrdenadas) {
        Estrella* estrella = par.second;
        
        // Proyección perspectiva: convertir 3D a 2D
        float factor = 200.0f / estrella->z; // Factor de perspectiva
        float screenX = centroX + estrella->x * factor;
        float screenY = centroY + estrella->y * factor;
        
        // Tamaño basado en la profundidad
        float escala = factor * 0.3f; // Ajustar tamaño base
        int ancho = static_cast<int>(anchoOriginal * escala);
        int alto = static_cast<int>(altoOriginal * escala);
        
        // Asegurar tamaño mínimo
        if (ancho < 1) ancho = 1;
        if (alto < 1) alto = 1;
        
        // Rectángulo de destino
        SDL_Rect destino;
        destino.x = static_cast<int>(screenX - ancho / 2);
        destino.y = static_cast<int>(screenY - alto / 2);
        destino.w = ancho;
        destino.h = alto;
        
        // Dibujar con rotación
        SDL_RenderCopyEx(renderer, estrella->textura, nullptr, &destino, 
                        estrella->rotacion, nullptr, SDL_FLIP_NONE);
    }
    
    SDL_RenderPresent(renderer);
}

// Game loop
void gameLoop() {
    SDL_Event event;
    bool corriendo = true;

    while (corriendo) {
        tiempo::setDeltaTime();

        // Comprobar entradas
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || 
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                corriendo = false;
            }
            
            // Efecto hiperespacio con espacio
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                hiperespacio = true;
            }
            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE) {
                hiperespacio = false;
            }
            
            // Redimensionado de la ventana
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_GetWindowSize(ventana::win, &ventana::width, &ventana::height);
            }
        }

        // Actualizar estrellas
        actualizarEstrellas();
        
        // Dibujar estrellas
        dibujarEstrellas();
    }
}

int main(int argc, char* argv[]) {
    // Inicializar
    inicializarSistema();
    
    // Inicializar estrellas
    inicializarEstrellas();
    
    // Game loop
    gameLoop();
    
    cerrarSDL();
    return 0;
}
