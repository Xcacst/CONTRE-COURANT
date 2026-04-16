// Filename: GameTest.cpp
// Provides a demo of how to use the API

#if BUILD_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <iostream>
#include <cstdio> // For sprintf
#include <math.h>
#include "../ContestAPI/app.h"
#include "../ContestAPI/freeglut_config.h"
#include <stdlib.h>
#include <time.h>

#include "Obstacle.h"          
#include "CollectableAlly.h"   
#include "Player.h"
#include "ParallaxSystem.h"
#include "SpawnManager.h"      
#include "GameConstants.h"
#include "UIManager.h"
#include "ParticleManager.h"

// ---------------------------------------------------------------------------
// GLOBALS
// ---------------------------------------------------------------------------

// GAME STATES
enum GameState
{
    MENU,
    PLAYING,
    WON,
    LOST
};

GameState g_currentState = MENU;
bool g_isDeepMusicPlaying = false;

// ENTITIES
Player* g_player = nullptr;
CSimpleSprite* boteEnemigo = nullptr;
CSimpleSprite* fondoOceano = nullptr;
ParallaxSystem* g_parallax = nullptr;
SpawnManager* g_spawnManager = nullptr;
UIManager* g_uiManager = nullptr;
ParticleManager* g_particleManager = nullptr;

// MAIN MENU
CSimpleSprite* menuSprite = nullptr;

// ENVIRONMENT
const int NUM_NUBES = 10;
CSimpleSprite* nubes[NUM_NUBES];
float velocidadNubes[NUM_NUBES];
float alturaOriginalNubes[NUM_NUBES];

// GAME VARIABLES
float g_profundidad = GameConfig::START_DEPTH;

// ---------------------------------------------------------------------------
// FUNCTIONS
// ---------------------------------------------------------------------------

void ResetGame()
{
    //deter music al reiniciar
    App::StopAudio("./data/audio/c.wav");
    App::StopAudio("./data/audio/a.wav");

    // Clean up entities
    if (g_player) { delete g_player; g_player = nullptr; }
    if (g_spawnManager) { g_spawnManager->Reset(); }

    // Reset Variables
    g_profundidad = GameConfig::START_DEPTH;
    g_isDeepMusicPlaying = false;

    if (g_parallax) {
        g_parallax->Reset();
    }

    if (g_particleManager) {
        g_particleManager->Reset();
    }

    g_player = new Player();
}

void Init()
{
    srand((unsigned int)time(NULL));

    g_parallax = new ParallaxSystem();
    g_parallax->Init();

    g_particleManager = new ParticleManager();
    g_particleManager->Init();

    g_spawnManager = new SpawnManager();
    g_uiManager = new UIManager();

    // 1. BACKGROUND
    fondoOceano = App::CreateSprite("./data/TestData/Fondo.bmp", 1, 1);
    fondoOceano->SetPosition(GameConfig::SCREEN_CENTER_X, GameConfig::OCEAN_BG_START_Y);
    fondoOceano->SetScale(0.9f);

    // 2. PLAYER
    g_player = new Player();

    // 3. BOAT
    boteEnemigo = App::CreateSprite("./data/TestData/BoteEnemigo.bmp", 1, 1);
    boteEnemigo->SetPosition(GameConfig::SCREEN_CENTER_X, GameConfig::BOAT_DISPLAY_Y);
    boteEnemigo->SetScale(0.1f);

    // 4. CLOUDS
    for (int i = 0; i < NUM_NUBES; i++)
    {
        nubes[i] = App::CreateSprite("./data/TestData/Nube.bmp", 1, 1);
        float posX = (float)(rand() % 1024);
        float posYBase = 500.0f + (float)(rand() % 200);
        alturaOriginalNubes[i] = posYBase;
        nubes[i]->SetPosition(posX, posYBase);

        float escala = 0.3f + ((float)(rand() % 50) / 100.0f);
        nubes[i]->SetScale(escala);

        velocidadNubes[i] = 0.10f + ((float)(rand() % 50) / 100.0f);
    }

    // 5. MENU SPRITE
    menuSprite = App::CreateSprite("./data/TestData/MainMenu.bmp", 1, 1);
    // Establecemos la posición en el centro de la pantalla (asumiendo 1024x768)
    menuSprite->SetPosition(512.0f, 384.0f);
    // IMPORTANTE: Modifica la escala de ser necesario dependiendo del tamaño original de tu imagen
    // Prueba con 0.5f (mitad del tamaño), si sigue grande bájalo a 0.3f, si es muy chico súbelo a 0.7f
    menuSprite->SetScale(0.5f); 

    g_currentState = MENU;
}

void UpdateMenu(float dt)
{
    if (App::IsKeyPressed(App::KEY_SPACE) || App::IsKeyPressed(App::KEY_ENTER))
    {
        ResetGame();
        g_currentState = PLAYING;
        App::PlayAudio("./data/audio/c.wav", true);
    }
}

void UpdatePlaying(float dt)
{
    float dtSec = dt / 1000.0f;

    // --- PHYSICS: TUG OF WAR ---
    float boatPullSpeed = GameConfig::BOAT_PULL_BASE_SPEED + (g_profundidad * GameConfig::BOAT_PULL_DEPTH_MULTIPLIER); 
    float playerSwimSpeed = 0.0f;

    if (g_player)
    {
        float prevX, prevY;
        g_player->GetPosition(prevX, prevY);

        g_player->Update(dt);

        if (g_player->IsSwimmingDown())
        {
            playerSwimSpeed = GameConfig::PLAYER_BASE_SWIM_SPEED + (g_player->GetMass() * GameConfig::PLAYER_MASS_SWIM_MULTIPLIER);
            
            // Spawn hydro-dynamic trail bubbles randomly while swimming down
            if (g_particleManager && (rand() % 100 < 30))
            {
                float px, py;
                g_player->GetPosition(px, py);
                g_particleManager->SpawnTrailBubble(px, py);
            }
        }
    }

    // Forces
    float netForce = playerSwimSpeed - boatPullSpeed;
    g_profundidad += netForce * dtSec;

    //musicchange
    if (g_profundidad > GameConfig::MUSIC_CHANGE_DEPTH && !g_isDeepMusicPlaying)
    {
        g_isDeepMusicPlaying = true;
        App::StopAudio("./data/audio/c.wav");
        App::PlayAudio("./data/audio/a.wav", true);
    }
    else if (g_profundidad <= GameConfig::MUSIC_CHANGE_DEPTH && g_isDeepMusicPlaying)
    {
        g_isDeepMusicPlaying = false;
        App::StopAudio("./data/audio/a.wav");
        App::PlayAudio("./data/audio/c.wav", true);
    }

    // --- WIN / LOSE CONDITIONS ---
    if (g_profundidad <= GameConfig::LOSE_DEPTH)
    {
        g_profundidad = GameConfig::LOSE_DEPTH;
        g_currentState = LOST;
        App::StopAudio("./data/audio/c.wav");
        App::StopAudio("./data/audio/a.wav");
    }
    if (g_profundidad >= GameConfig::WIN_DEPTH)
    {
        g_profundidad = GameConfig::WIN_DEPTH;
        g_currentState = WON;
        App::StopAudio("./data/audio/c.wav");
        App::StopAudio("./data/audio/a.wav");
    }

    if (g_spawnManager)
    {
        g_spawnManager->Update(dt, netForce, g_profundidad);
        g_spawnManager->CheckCollisions(g_player);
    }

    if (g_parallax)
    {
        g_parallax->CheckCollisions(g_player);
    }

    if (g_uiManager)
    {
        g_uiManager->SetHighScore(g_profundidad);
        g_uiManager->Update(dt);
    }

    if (g_parallax)
    {
        g_parallax->Update(netForce, dt);
    }

    if (g_particleManager)
    {
        g_particleManager->Update(dt, netForce);
    }

    // --- VISUALS ---
    fondoOceano->SetPosition(GameConfig::SCREEN_CENTER_X, GameConfig::OCEAN_BG_START_Y + g_profundidad);

    static float waveTime = 0.0f;
    waveTime += dtSec;
    float boatY = GameConfig::BOAT_DISPLAY_Y + sinf(waveTime * 2.0f) * 10.0f + g_profundidad;
    boteEnemigo->SetPosition(GameConfig::SCREEN_CENTER_X, boatY);

    for (int i = 0; i < NUM_NUBES; i++)
    {
        float x, y;
        nubes[i]->GetPosition(x, y);
        x += velocidadNubes[i];
        y = alturaOriginalNubes[i] + g_profundidad;
        if (x > 1100.0f) x = -100.0f;
        nubes[i]->SetPosition(x, y);
        nubes[i]->Update(dt);
    }
}

void UpdateGameOver(float dt)
{
    if (App::IsKeyPressed(App::KEY_R))
    {
        ResetGame();
        g_currentState = PLAYING;
        App::PlayAudio("./data/audio/c.wav", true);
    }
}

void Update(const float deltaTime)
{
    switch (g_currentState)
    {
    case MENU: UpdateMenu(deltaTime); break;
    case PLAYING: UpdatePlaying(deltaTime); break;
    case WON:
    case LOST: UpdateGameOver(deltaTime); break;
    }
}

// ---------------------------------------------------------------------------
// RENDER
// ---------------------------------------------------------------------------
void Render()
{
    // === 1. PREPARAR EL SCREEN SHAKE (MANIPULACIÓN DE MATRIZ) ===
    float shakeX = 0.0f;
    float shakeY = 0.0f;

    if (g_currentState == PLAYING && g_player)
    {
        float tensionFactor = 1.0f - (g_player->GetStamina() / 100.0f);
        if (tensionFactor > 0.9f)
        {
            // Calculamos un desplazamiento tenue en Coordenadas de Dispositivo Normalizadas (NDC)
            shakeX = (((rand() % 100) / 100.0f) * 0.02f) - 0.01f;
            shakeY = (((rand() % 100) / 100.0f) * 0.02f) - 0.01f;
        }
    }

    // Agitamos la cámara entera
    glPushMatrix();
    glTranslatef(shakeX, shakeY, 0.0f);

    // === 2. DIBUJAR ELEMENTOS DEL JUEGO ===
    fondoOceano->Draw();

    if (g_parallax)
    {
        g_parallax->Render();
    }

    // Burbujas: encima del parallax, detrás de todos los sprites
    if (g_particleManager)
    {
        g_particleManager->Draw();
    }

    for (int i = 0; i < NUM_NUBES; i++) nubes[i]->Draw();

    if (g_currentState == PLAYING)
    {
        if (g_spawnManager) g_spawnManager->Draw();

        boteEnemigo->Draw();

        if (g_player)
        {
            float px, py, bx, by;
            g_player->GetPosition(px, py);
            boteEnemigo->GetPosition(bx, by);

            float tensionFactor = 1.0f - (g_player->GetStamina() / 100.0f);
            
            App::DrawLine(px + 10.0f, py + 10.0f, bx, by, tensionFactor, 1.0f - tensionFactor, 0.0f);

            if (g_uiManager) 
            {
                g_uiManager->RenderVignette(g_player, g_profundidad);
                g_uiManager->RenderFlash();
            }

            g_player->Draw();
        }
    }

    // === 3. RESTAURAR LA MATRIZ DE LA CÁMARA ===
    glPopMatrix();

    // === 4. DIBUJAR UI / HUD (NO DEBEN TEMBLAR) ===
    if (g_currentState == PLAYING && g_player)
    {
        if (g_uiManager) 
        {
            g_uiManager->RenderHUD(g_player, g_profundidad, GameConfig::WIN_DEPTH);
            g_uiManager->RenderDepthBar(g_profundidad, GameConfig::WIN_DEPTH);
        }
    }
    else if (g_currentState == MENU)
    {
        if (menuSprite) menuSprite->Draw();
        if (g_uiManager) g_uiManager->RenderMenu();
    }
    else if (g_currentState == WON)
    {
        if (g_uiManager) g_uiManager->RenderWon();
    }
    else if (g_currentState == LOST)
    {
        if (g_uiManager) g_uiManager->RenderLost();
    }
}

// ---------------------------------------------------------------------------
// SHUTDOWN
// ---------------------------------------------------------------------------
void Shutdown()
{
    if (g_parallax) { delete g_parallax; g_parallax = nullptr; }
    if (g_particleManager) { delete g_particleManager; g_particleManager = nullptr; }
    if (boteEnemigo) delete boteEnemigo;
    if (fondoOceano) delete fondoOceano;
    if (g_player) delete g_player;
    if (g_spawnManager) { delete g_spawnManager; g_spawnManager = nullptr; }
    if (g_uiManager) { delete g_uiManager; g_uiManager = nullptr; }
    if (menuSprite) { delete menuSprite; menuSprite = nullptr; }

    for (int i = 0; i < NUM_NUBES; i++) if (nubes[i]) delete nubes[i];
}