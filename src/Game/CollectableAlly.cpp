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
    
    m_state = AllyState::PASSING;
    m_facingRight = (m_speedX >= 0); 
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
    
    float newX = m_x + (sinf(time * 3.0f + m_x) * m_speedX * dtSec * 100.0f);
    m_x = newX;

   
    if (m_x < GameConfig::GAME_MIN_X && !m_facingRight)
    {
        m_facingRight = true;
    }
    else if (m_x > GameConfig::GAME_MAX_X && m_facingRight)
    {
        m_facingRight = false;
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
