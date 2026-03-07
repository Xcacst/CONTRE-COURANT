// Filename: GameTest.cpp
// Provides a demo of how to use the API

#if BUILD_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <iostream>
#include <cstdio> // For sprintf
#include <math.h>
#include "../ContestAPI/app.h"
#include <stdlib.h>
#include <time.h>

#include "Player.h"
#include "Obstacle.h"
#include "CollectableAlly.h" 
#include "ParallaxSystem.h"
#include <vector>
#include <algorithm> // For std::remove_if

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

std::vector<Obstacle*> g_obstacles;
float g_obstacleSpawnTimer = 0.0f;

std::vector<CollectableAlly*> g_allies;
float g_allySpawnTimer = 0.0f;

// ENVIRONMENT
const int NUM_NUBES = 10;
CSimpleSprite* nubes[NUM_NUBES];
float velocidadNubes[NUM_NUBES];
float alturaOriginalNubes[NUM_NUBES];

// GAME VARIABLES
float g_profundidad = 400.0f;
const float MAX_PROFUNDIDAD = 2300.0f;
const float MIN_PROFUNDIDAD = 0.0f;

// Helper: Simple AABB Collision
bool IsColliding(GameObject* a, GameObject* b)
{
    float ax, ay, bx, by;
    a->GetPosition(ax, ay);
    b->GetPosition(bx, by);

    float aw = a->GetWidth() * a->GetSprite()->GetScale();
    float ah = a->GetHeight() * a->GetSprite()->GetScale();
    float bw = b->GetWidth() * b->GetSprite()->GetScale();
    float bh = b->GetHeight() * b->GetSprite()->GetScale();

    return (abs(ax - bx) < (aw / 2.0f + bw / 2.0f - 10.0f)) &&
        (abs(ay - by) < (ah / 2.0f + bh / 2.0f - 10.0f));
}

// ---------------------------------------------------------------------------
// FUNCTIONS
// ---------------------------------------------------------------------------

void ResetGame()
{
    //deter music al reiniciar
    App::StopAudio("./data/audio/b.wav");
    App::StopAudio("./data/audio/a.wav");
    // Clean up entities
    if (g_player) { delete g_player; g_player = nullptr; }
    for (auto obs : g_obstacles) delete obs;
    g_obstacles.clear();
    for (auto ally : g_allies) delete ally;
    g_allies.clear();

    // Reset Variables
    g_profundidad = 400.0f;
    g_obstacleSpawnTimer = 0.0f;
    g_allySpawnTimer = 0.0f;
    g_isDeepMusicPlaying = false;

    if (g_parallax) {
        g_parallax->Reset();
    }

    g_player = new Player();
}

void Init()
{
    srand((unsigned int)time(NULL));

    g_parallax = new ParallaxSystem();
    g_parallax->Init();

    // 1. BACKGROUND
    fondoOceano = App::CreateSprite("./data/TestData/Fondo.bmp", 1, 1);
    fondoOceano->SetPosition(512.0f, -800.0f);
    fondoOceano->SetScale(0.9f);

    // 2. PLAYER
    g_player = new Player();

    // 3. BOAT
    boteEnemigo = App::CreateSprite("./data/TestData/BoteEnemigo.bmp", 1, 1);
    boteEnemigo->SetPosition(512.0f, 300.0f);
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

    g_currentState = MENU;
}

void UpdateMenu(float dt)
{
    if (App::IsKeyPressed(App::KEY_SPACE) || App::IsKeyPressed(App::KEY_ENTER))
    {
        ResetGame();
        g_currentState = PLAYING;
        App::PlayAudio("./data/audio/b.wav", true);
    }
}

void UpdatePlaying(float dt)
{
    float dtSec = dt / 1000.0f;

    // --- PHYSICS: TUG OF WAR ---
    float boatPullSpeed = 80.0f + (g_profundidad * 0.015f); // Pull gets harder deeper you go
    float playerSwimSpeed = 0.0f;

    if (g_player)
    {
        float prevX, prevY;
        g_player->GetPosition(prevX, prevY);

        g_player->Update(dt);

        if (g_player->IsSwimmingDown())
        {
            playerSwimSpeed = 200.0f + (g_player->GetMass() * 5.0f);
        }
    }

    // Forces
    float netForce = playerSwimSpeed - boatPullSpeed;
    g_profundidad += netForce * dtSec;

    //musicchange
    const float DEEP_MUSIC_THRESHOLD = 1200.0f;
    if (g_profundidad > DEEP_MUSIC_THRESHOLD && !g_isDeepMusicPlaying)
    {
        g_isDeepMusicPlaying = true;
        App::StopAudio("./data/audio/b.wav");
        App::PlayAudio("./data/audio/a.wav", true);
    }
    else if (g_profundidad <= DEEP_MUSIC_THRESHOLD && g_isDeepMusicPlaying)
    {
        g_isDeepMusicPlaying = false;
        App::StopAudio("./data/audio/a.wav");
        App::PlayAudio("./data/audio/b.wav", true);
    }

    // --- WIN / LOSE CONDITIONS ---
    if (g_profundidad <= MIN_PROFUNDIDAD)
    {
        g_profundidad = MIN_PROFUNDIDAD;
        g_currentState = LOST;
        App::StopAudio("./data/audio/b.wav");
        App::StopAudio("./data/audio/a.wav");
    }
    if (g_profundidad >= MAX_PROFUNDIDAD)
    {
        g_profundidad = MAX_PROFUNDIDAD;
        g_currentState = WON;
        App::StopAudio("./data/audio/b.wav");
        App::StopAudio("./data/audio/a.wav");
    }

    // --- MANAGERS ---

    // Obstacles
    g_obstacleSpawnTimer += dtSec;
    if (g_obstacleSpawnTimer > 2.0f)
    {
        g_obstacleSpawnTimer = 0.0f;
        float spawnX = (float)(rand() % 900 + 50);
        float spawnY = -200.0f;

        // CAMBIO INTEGRADO: Elegir entre 3 tipos para que aparezca la lata
        int r = rand() % 3;
        ObstacleType type;
        if (r == 0) type = TRASH_PLASTIC_BAG;
        else if (r == 1) type = TRASH_NET;
        else type = TRASH_CAN;

        g_obstacles.push_back(new Obstacle(spawnX, spawnY, type));
    }

    // Obstacle Logic
    for (size_t i = 0; i < g_obstacles.size(); i++)
    {
        Obstacle* obs = g_obstacles[i];
        if (!obs->IsActive()) continue;

        float relativeSpeed = netForce;
        float ox, oy;
        obs->GetPosition(ox, oy);
        oy += relativeSpeed * dtSec;
        oy += 30.0f * dtSec;

        obs->SetPosition(ox, oy);
        obs->Update(dt);

        if (g_player && IsColliding(g_player, obs))
        {
            obs->Deactivate();
            // Lógica de penalización actualizada
            if (obs->GetType() == TRASH_PLASTIC_BAG) g_player->ReduceStamina(20.0f);
            else if (obs->GetType() == TRASH_NET) g_player->ReduceStamina(10.0f);
            else if (obs->GetType() == TRASH_CAN) g_player->ReduceStamina(30.0f); // Penalización mayor para la lata
        }

        if (oy > 1000.0f || oy < -500.0f) obs->Deactivate();
    }

    // Clean Obstacles
    g_obstacles.erase(std::remove_if(g_obstacles.begin(), g_obstacles.end(),
        [](Obstacle* obs) { if (!obs->IsActive()) { delete obs; return true; } return false; }), g_obstacles.end());

    // Allies
    g_allySpawnTimer += dtSec;
    if (g_allySpawnTimer > 3.0f)
    {
        g_allySpawnTimer = 0.0f;
        float spawnX = (float)(rand() % 900 + 50);
        float spawnY = -200.0f;
        g_allies.push_back(new CollectableAlly(spawnX, spawnY, "./data/TestData/PEZ_DER.bmp", "./data/TestData/PEZ_IZQ.bmp"));
    }

    // Ally Logic
    for (auto it = g_allies.begin(); it != g_allies.end(); )
    {
        CollectableAlly* ally = *it;
        float relativeSpeed = netForce;
        float ax, ay;
        ally->GetPosition(ax, ay);
        ay += relativeSpeed * dtSec;
        ally->SetPosition(ax, ay);
        ally->Update(dt);

        if (g_player && IsColliding(g_player, ally))
        {
            g_player->AddAllyMass(ally->GetMassValue());
            g_player->RecoverStamina(10.0f);
            g_player->AddAllyToSwarm(ally);
            it = g_allies.erase(it);
        }
        else if (ay > 1000.0f || ay < -500.0f)
        {
            delete ally;
            it = g_allies.erase(it);
        }
        else ++it;
    }
    if (g_parallax)
    {
        g_parallax->Update(netForce, dt);
    }

    

    // --- VISUALS ---
    fondoOceano->SetPosition(512.0f, -800.0f + g_profundidad);

    static float waveTime = 0.0f;
    waveTime += dtSec;
    float boatY = 300.0f + sinf(waveTime * 2.0f) * 10.0f + g_profundidad;
    boteEnemigo->SetPosition(512.0f, boatY);

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
        App::PlayAudio("./data/audio/b.wav", true);
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
    
    fondoOceano->Draw();
  if (g_parallax)
    {
        g_parallax->Render();
    }

    for (int i = 0; i < NUM_NUBES; i++) nubes[i]->Draw();


    if (g_currentState == PLAYING)
    {
      
        for (auto obs : g_obstacles) if (obs->IsActive()) obs->Draw();
        for (auto ally : g_allies) ally->Draw();

        boteEnemigo->Draw();

        if (g_player)
        {
            float px, py, bx, by;
            g_player->GetPosition(px, py);
            boteEnemigo->GetPosition(bx, by);

            float tensionFactor = 1.0f - (g_player->GetStamina() / 100.0f);
            float shake = (tensionFactor > 0.9f) ? (rand() % 5 - 2.5f) : 1.0f;
            App::DrawLine(px + 10.0f, py + 10.0f, bx, by, tensionFactor, 1.0f - tensionFactor, 0.0f);

            g_player->Draw();

            // --- HUD ---
            char buffer[64];
            sprintf(buffer, "DEPTH: %.1f m / %.1f m", g_profundidad / 10.0f, MAX_PROFUNDIDAD / 10.0f);
            App::Print(20, 740, buffer, 1.0f, 1.0f, 1.0f);

            sprintf(buffer, "STAMINA: %.0f%%", g_player->GetStamina());
            App::Print(20, 720, buffer, tensionFactor, 1.0f - tensionFactor, 0.0f);

            sprintf(buffer, "MASS: %.1f kg", g_player->GetMass());
            App::Print(20, 700, buffer, 0.5f, 0.5f, 1.0f);
        }
    }
    else if (g_currentState == MENU)
    {
        App::Print(300, 400, "CONTRE-COURANT", 1.0f, 1.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24);
        App::Print(320, 350, "Press SPACE to Start", 1.0f, 1.0f, 1.0f);
        App::Print(320, 320, "Arrows to Swim, DOWN to Dive", 0.8f, 0.8f, 0.8f);
    }
    else if (g_currentState == WON)
    {
        App::Print(350, 400, "VICTORY!", 0.0f, 1.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24);
        App::Print(300, 350, "The boat has been sunk!", 1.0f, 1.0f, 1.0f);
        App::Print(320, 300, "Press R to Restart", 0.8f, 0.8f, 0.8f);
    }
    else if (g_currentState == LOST)
    {
        App::Print(350, 400, "GAME OVER", 1.0f, 0.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24);
        App::Print(300, 350, "You were caught!", 1.0f, 1.0f, 1.0f);
        App::Print(320, 300, "Press R to Restart", 0.8f, 0.8f, 0.8f);
    }
}

// ---------------------------------------------------------------------------
// SHUTDOWN
// ---------------------------------------------------------------------------
void Shutdown()
{
    if (g_parallax) {
        delete g_parallax; g_parallax = nullptr;
    }
    if (boteEnemigo) delete boteEnemigo;
    if (fondoOceano) delete fondoOceano;
    if (g_player) delete g_player;


    for (int i = 0; i < NUM_NUBES; i++) if (nubes[i]) delete nubes[i];
    for (auto obs : g_obstacles) delete obs;
    for (auto ally : g_allies) delete ally;
}