#include "ParticleManager.h"
#include "GameConstants.h"

#if BUILD_PLATFORM_WINDOWS
#include <windows.h>
#endif
#include "../ContestAPI/freeglut_config.h"

#include <stdlib.h>  // rand
#include <math.h>    // sinf

// ---------------------------------------------------------------------------
// Helper de rango aleatorio
// ---------------------------------------------------------------------------
static float RandFloat(float lo, float hi)
{
    return lo + ((float)(rand() % 10000) / 10000.0f) * (hi - lo);
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
ParticleManager::ParticleManager()
    : m_time(0.0f), m_trailIndex(0)
{
    for (int i = 0; i < GameConfig::BUBBLE_MAX_COUNT; ++i)
        m_bubbles[i] = {};
    for (int i = 0; i < 50; ++i)
        m_trailBubbles[i] = {};
}

// ---------------------------------------------------------------------------
// Init — reparte las burbujas por toda la pantalla al arrancar
// ---------------------------------------------------------------------------
void ParticleManager::Init()
{
    m_time = 0.0f;
    m_trailIndex = 0;
    for (int i = 0; i < 50; ++i) m_trailBubbles[i].active = false;

    for (int i = 0; i < GameConfig::BUBBLE_MAX_COUNT; ++i)
    {
        float startX = RandFloat(GameConfig::GAME_MIN_X, GameConfig::GAME_MAX_X);
        float startY = RandFloat(-100.0f, 700.0f);
        SpawnBubbleAt(i, startX, startY);
    }
}

// ---------------------------------------------------------------------------
// Reset — igual que Init (sin new/delete)
// ---------------------------------------------------------------------------
void ParticleManager::Reset()
{
    Init();
}

// ---------------------------------------------------------------------------
// SpawnBubbleAt — rellena los datos de una burbuja en el índice dado
// ---------------------------------------------------------------------------
void ParticleManager::SpawnBubbleAt(int index, float x, float y)
{
    Bubble& b = m_bubbles[index];
    b.x       = x;
    b.y       = y;
    b.speedY  = RandFloat(30.0f, 90.0f);
    b.size    = RandFloat(GameConfig::BUBBLE_SIZE_MIN, GameConfig::BUBBLE_SIZE_MAX);
    b.alpha   = RandFloat(GameConfig::BUBBLE_ALPHA_MIN, GameConfig::BUBBLE_ALPHA_MAX);
    b.phase   = RandFloat(0.0f, 6.28318f);
    b.active  = true;
}

// ---------------------------------------------------------------------------
// SpawnTrailBubble — Spawns a bubble at the player's position with negative Y speed
// ---------------------------------------------------------------------------
void ParticleManager::SpawnTrailBubble(float x, float y)
{
    Bubble& b = m_trailBubbles[m_trailIndex];
    b.x       = x + RandFloat(-5.0f, 5.0f);
    b.y       = y + RandFloat(-5.0f, 5.0f);
    b.speedY  = RandFloat(-150.0f, -50.0f); // Velocidad inicial NEGATIVA (baja con el jugador)
    b.size    = RandFloat(GameConfig::BUBBLE_SIZE_MIN, GameConfig::BUBBLE_SIZE_MAX);
    b.alpha   = RandFloat(GameConfig::BUBBLE_ALPHA_MIN, GameConfig::BUBBLE_ALPHA_MAX);
    b.phase   = RandFloat(0.0f, 6.28318f);
    b.active  = true;

    m_trailIndex = (m_trailIndex + 1) % 50;
}

// ---------------------------------------------------------------------------
// Update
//   dt       — delta time en milisegundos
//   netForce — playerSwimSpeed - boatPullSpeed (>0 cuando el pez empuja)
// ---------------------------------------------------------------------------
void ParticleManager::Update(float dt, float netForce)
{
    const float dtSec = dt / 1000.0f;
    m_time += dtSec;

    float forceBoost = (netForce > 0.0f) ? netForce * GameConfig::BUBBLE_FORCE_SCALE : 0.0f;

    for (int i = 0; i < GameConfig::BUBBLE_MAX_COUNT; ++i)
    {
        Bubble& b = m_bubbles[i];
        if (!b.active) continue;

        // Subida con boost de fuerza
        b.y += (b.speedY + forceBoost) * dtSec;

        // Zigzag senoidal lateral
        b.x += sinf(m_time * GameConfig::BUBBLE_ZIGZAG_FREQ + b.phase)
               * GameConfig::BUBBLE_ZIGZAG_AMP * dtSec;

        // Reciclado circular: desaparece antes del barco (Y=300) → vuelve por abajo
        if (b.y > GameConfig::DESPAWN_Y_TOP)
        {
            float newX = RandFloat(GameConfig::GAME_MIN_X, GameConfig::GAME_MAX_X);
            SpawnBubbleAt(i, newX, GameConfig::SPAWN_START_Y);
        }
    }

    // Actualizar burbujas de la estela
    for (int i = 0; i < 50; ++i)
    {
        Bubble& b = m_trailBubbles[i];
        if (!b.active) continue;

        // Aceleración positiva (flotabilidad) contrarresta la inercia negativa inicial
        b.speedY += 250.0f * dtSec; // Flotabilidad gravitacional

        b.y += b.speedY * dtSec;
        b.x += sinf(m_time * GameConfig::BUBBLE_ZIGZAG_FREQ + b.phase) * GameConfig::BUBBLE_ZIGZAG_AMP * dtSec;

        // Desactivar si llega muy arriba
        if (b.y > GameConfig::DESPAWN_Y_TOP)
        {
            b.active = false;
        }
    }
}

// ---------------------------------------------------------------------------
// Draw — GL_QUADS pequeños con alpha blending
//   La ContestAPI usa coords virtuales 1024x768 → convertimos a NDC (-1..1)
// ---------------------------------------------------------------------------
void ParticleManager::Draw() const
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const float invW = 2.0f / 1024.0f;
    const float invH = 2.0f / 768.0f;

    glBegin(GL_QUADS);
    for (int i = 0; i < GameConfig::BUBBLE_MAX_COUNT; ++i)
    {
        const Bubble& b = m_bubbles[i];
        if (!b.active) continue;

        glColor4f(0.75f, 0.92f, 1.0f, b.alpha); // celeste agüita translúcido

        // Convertir coords virtuales a NDC
        float cx = b.x * invW - 1.0f;
        float cy = b.y * invH - 1.0f;
        float hw = b.size * invW * 0.5f;
        float hh = b.size * invH * 0.5f;

        glVertex2f(cx - hw, cy - hh);
        glVertex2f(cx + hw, cy - hh);
        glVertex2f(cx + hw, cy + hh);
        glVertex2f(cx - hw, cy + hh);
    }

    // Dibujar estela
    for (int i = 0; i < 50; ++i)
    {
        const Bubble& b = m_trailBubbles[i];
        if (!b.active) continue;

        glColor4f(0.75f, 0.92f, 1.0f, b.alpha);

        float cx = b.x * invW - 1.0f;
        float cy = b.y * invH - 1.0f;
        float hw = b.size * invW * 0.5f;
        float hh = b.size * invH * 0.5f;

        glVertex2f(cx - hw, cy - hh);
        glVertex2f(cx + hw, cy - hh);
        glVertex2f(cx + hw, cy + hh);
        glVertex2f(cx - hw, cy + hh);
    }

    glEnd();

    glDisable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // restaurar color por defecto
}
