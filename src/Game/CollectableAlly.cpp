#include "CollectableAlly.h"
#include "../ContestAPI/app.h" // For App::CreateSprite
#include "GameConstants.h"
#include <stdlib.h> // rand
#include <math.h>   // sinf

CollectableAlly::CollectableAlly(float startX, float startY, const char* spriteRightFile, const char* spriteLeftFile)
    : GameObject(spriteRightFile, 1, 1) 
{
    m_spriteRight = m_sprite; 
    m_spriteLeft = App::CreateSprite(spriteLeftFile, 1, 1); 
    // Asegurarse que ambos sprites tengan la misma escala inicial
    m_spriteLeft->SetScale(GetSprite()->GetScale());

    SetPosition(startX, startY);
    SetScale(GameConfig::ALLY_SCALE); 

    m_massValue = GameConfig::ALLY_MASS_VALUE; 
    m_speedX = (float)(rand() % 100 - 50) / 100.0f; 
    m_speedY = (float)(rand() % 50) / 100.0f + 0.1f; 
    
    // Inicializar velocidad (Boids) con una pequeña velocidad al azar
    m_velocityX = m_speedX * GameConfig::BOIDS_MAX_SPEED;
    m_velocityY = m_speedY * GameConfig::BOIDS_MAX_SPEED * 0.2f;

    m_state = AllyState::PASSING;
    m_facingRight = (m_velocityX >= 0); 
    m_offsetX = 0.0f;
    m_offsetY = 0.0f;
}

CollectableAlly::~CollectableAlly()
{
  
    
   
    if (m_spriteLeft && m_spriteLeft != m_spriteRight)
    {
        delete m_spriteLeft;
        m_spriteLeft = nullptr;
    }

   
    if (m_spriteRight)
    {
        delete m_spriteRight;
        m_spriteRight = nullptr;
    }

    
    m_sprite = nullptr;
}

void CollectableAlly::Update(float dt)
{
    switch (m_state)
    {
    case AllyState::PASSING:
        UpdatePassing(dt);
        break;
    case AllyState::JOINING:
    
        break;
    case AllyState::ESCORTE:
        UpdateEscorte(dt);
        break;
    }
    
    m_sprite = m_facingRight ? m_spriteRight : m_spriteLeft;
    GameObject::Update(dt);
}

void CollectableAlly::Draw()
{
   
    GameObject::Draw();
}

void CollectableAlly::UpdatePassing(float dt)
{
    float dtSec = dt / 1000.0f;

    static float time = 0.0f;
    time += dtSec;
    
    // Movimiento natural base (ondulante) sumado a la velocidad boids
    // Usamos m_speedX como un valor aleatorio de fase para que no todos naden igual
    float phaseOffset = m_speedX * 10.0f;
    float naturalWobbleX = sinf(time * 2.0f + phaseOffset) * 15.0f * dtSec;
    float naturalWobbleY = cosf(time * 1.5f + phaseOffset) * 10.0f * dtSec;

    m_x += (m_velocityX * dtSec) + naturalWobbleX;
    m_y += (m_velocityY * dtSec) + naturalWobbleY;
    
    // Limitar velocidad máxima 
    float speedSq = m_velocityX * m_velocityX + m_velocityY * m_velocityY;
    if (speedSq > GameConfig::BOIDS_MAX_SPEED * GameConfig::BOIDS_MAX_SPEED && speedSq > 0)
    {
        float multiplier = GameConfig::BOIDS_MAX_SPEED / sqrt(speedSq);
        m_velocityX *= multiplier;
        m_velocityY *= multiplier;
    }

    // Tendencia a subir lentamente si van muy abajo (para quedarse en la zona de juego)
    if (m_y < GameConfig::GAME_DESPAWN_Y - 300.0f) {
        m_velocityY += 5.0f * dtSec;
    }

    // Rebotar en los bordes de la pantalla suavemente (evitar que se fuguen lateralmente)
    if (m_x < GameConfig::GAME_MIN_X + 50.0f)
    {
        m_velocityX += 15.0f * dtSec;
        m_facingRight = true;
    }
    else if (m_x > GameConfig::GAME_MAX_X - 50.0f)
    {
        m_velocityX -= 15.0f * dtSec;
        m_facingRight = false;
    }
    else
    {
        // Si no choca con las paredes, la direccion se basa en su velocidad X constante
        if (m_velocityX > 5.0f) m_facingRight = true;
        if (m_velocityX < -5.0f) m_facingRight = false;
    }
}

void CollectableAlly::ApplyBoids(const std::vector<CollectableAlly*>& allies)
{
    if (m_state != AllyState::PASSING) return;

    float cohesionX = 0, cohesionY = 0;
    float alignmentX = 0, alignmentY = 0;
    float separationX = 0, separationY = 0;

    int neighborCount = 0;
    int separationCount = 0;

    for (CollectableAlly* other : allies)
    {
        if (other == this || other->GetState() != AllyState::PASSING) continue;

        float dx = other->m_x - m_x;
        float dy = other->m_y - m_y;
        float distSq = dx*dx + dy*dy;

        if (distSq < GameConfig::BOIDS_NEIGHBOR_RADIUS * GameConfig::BOIDS_NEIGHBOR_RADIUS)
        {
            cohesionX += other->m_x;
            cohesionY += other->m_y;
            
            alignmentX += other->m_velocityX;
            alignmentY += other->m_velocityY;
            
            neighborCount++;

            if (distSq < GameConfig::BOIDS_SEPARATION_RADIUS * GameConfig::BOIDS_SEPARATION_RADIUS && distSq > 0.001f)
            {
                separationX -= dx / sqrt(distSq); // Fuerza de empuje inversamente prop a la distancia no es realista a veces pero está bien para un juego basico, dividimos entre dist (normalizado negativo)
                separationY -= dy / sqrt(distSq);
                separationCount++;
            }
        }
    }

    if (neighborCount > 0)
    {
        // Cohesión: ir al centro de masa
        cohesionX /= neighborCount;
        cohesionY /= neighborCount;
        
        // Convertir en un vector que apunta hacia allá
        float steerCohesionX = cohesionX - m_x;
        float steerCohesionY = cohesionY - m_y;

        // Alineación: promediar velocidades
        alignmentX /= neighborCount;
        alignmentY /= neighborCount;

        // Aplicar pesos (multiplicado por un factor de sensibilidad bajo en vez de dt ya que ApplyBoids se llama en Update, o lo escalamos)
        m_velocityX += steerCohesionX * GameConfig::BOIDS_COHESION_WEIGHT * 0.005f;
        m_velocityY += steerCohesionY * GameConfig::BOIDS_COHESION_WEIGHT * 0.005f;

        m_velocityX += alignmentX * GameConfig::BOIDS_ALIGNMENT_WEIGHT * 0.01f;
        m_velocityY += alignmentY * GameConfig::BOIDS_ALIGNMENT_WEIGHT * 0.01f;
    }

    if (separationCount > 0)
    {
        separationX /= separationCount;
        separationY /= separationCount;

        m_velocityX += separationX * GameConfig::BOIDS_SEPARATION_WEIGHT * 5.0f;
        m_velocityY += separationY * GameConfig::BOIDS_SEPARATION_WEIGHT * 5.0f;
    }
}

void CollectableAlly::UpdateEscorte(float dt)
{
    
}

void CollectableAlly::SetEscortOffset(float offsetX, float offsetY)
{
    m_offsetX = offsetX;
    m_offsetY = offsetY;
}

void CollectableAlly::UpdateEscortPosition(float playerX, float playerY, float dt)
{
    
    float targetX = playerX + m_offsetX;
    float targetY = playerY + m_offsetY;

    float lerpFactor = GameConfig::ALLY_LERP_FACTOR; 
    m_x = m_x + (targetX - m_x) * lerpFactor;
    m_y = m_y + (targetY - m_y) * lerpFactor;

  
    m_facingRight = (m_x < playerX);
}
