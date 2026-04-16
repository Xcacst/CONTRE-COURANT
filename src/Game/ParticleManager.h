#ifndef _PARTICLE_MANAGER_H
#define _PARTICLE_MANAGER_H

namespace GameConfig
{
    constexpr int   BUBBLE_MAX_COUNT    = 150;
    constexpr float BUBBLE_BASE_SPEED   = 60.0f;   // px/s subiendo
    constexpr float BUBBLE_FORCE_SCALE  = 0.8f;    // cuánto amplifica el netForce
    constexpr float BUBBLE_ALPHA_MIN    = 0.08f;
    constexpr float BUBBLE_ALPHA_MAX    = 0.35f;
    constexpr float BUBBLE_SIZE_MIN     = 2.0f;
    constexpr float BUBBLE_SIZE_MAX     = 5.0f;
    constexpr float BUBBLE_ZIGZAG_AMP   = 18.0f;   // amplitud del seno lateral
    constexpr float BUBBLE_ZIGZAG_FREQ  = 1.8f;    // frecuencia del seno
}

// ---------------------------------------------------------------------------
// Burbuja individual — datos puros, sin punteros, sin heap
// ---------------------------------------------------------------------------
struct Bubble
{
    float x;
    float y;
    float speedY;     // velocidad base de subida (px/s)
    float size;       // tamaño del quad en píxeles
    float alpha;      // transparencia [0..1]
    float phase;      // desfase para el zigzag senoidal
    bool  active;
};

// ---------------------------------------------------------------------------
// ParticleManager — pool estático de burbujas
// ---------------------------------------------------------------------------
class ParticleManager
{
public:
    ParticleManager();

    // Inicializa las burbujas repartidas por toda la pantalla
    void Init();

    // dt en ms, netForce = playerSwimSpeed - boatPullSpeed
    void Update(float dt, float netForce);

    // Dibuja con GL_QUADS + alpha blending
    void Draw() const;

    // Reinicio instantáneo sin new/delete
    void Reset();

private:
    Bubble m_bubbles[GameConfig::BUBBLE_MAX_COUNT];
    Bubble m_trailBubbles[50];
    int    m_trailIndex;
    
    float  m_time; // tiempo acumulado para el seno

    void SpawnBubbleAt(int index, float x, float y);
public:
    void SpawnTrailBubble(float x, float y);
};

#endif // _PARTICLE_MANAGER_H
