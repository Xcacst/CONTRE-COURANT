#include "Player.h"
#include "../ContestAPI/app.h"
#include "CollectableAlly.h" 
#include <algorithm> 
#include <math.h>


Player::Player() : GameObject("./data/TestData/PEZ_DER.bmp", 1, 1)
{
    // Cargar sprites direccionales
    m_spriteRight = m_sprite; // El inicial es el derecho
    m_spriteLeft = App::CreateSprite("./data/TestData/PEZ_IZQ.bmp", 1, 1);
    
    m_facingRight = true;

    m_mass = 10.0f;       // Masss
    m_stamina = 100.0f;   // Energy
    m_speedX = 300.0f;    // V


    m_isSwimmingDown = false;
    
    m_swarmRotationAngle = 0.0f;
    m_swarmTime = 0.0f;

    SetScale(0.1f);
    SetPosition(512.0f, 200.0f); 
}

Player::~Player()
{
    // Liberar la memoria de los aliados en el enjambre
    for (auto ally : m_swarm)
    {
        delete ally;
    }
    m_swarm.clear();

    // Liberar sprites del jugador de forma segura (evitar Double Free)
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

void Player::Update(float dt)
{
    float dtSec = dt / 1000.0f;

   
    m_isSwimmingDown = false;

    
    if (App::IsKeyPressed(App::KEY_LEFT) || App::IsKeyPressed(App::KEY_A))
    {
        m_x -= m_speedX * dtSec;
        m_facingRight = false; 
    }
    if (App::IsKeyPressed(App::KEY_RIGHT) || App::IsKeyPressed(App::KEY_D))
    {
        m_x += m_speedX * dtSec;
        m_facingRight = true; 
    }

   
    if (m_x < 50.0f) m_x = 50.0f;
    if (m_x > 974.0f) m_x = 974.0f;

    // 2. Mecánica (Key DOWN)
    if (App::IsKeyPressed(App::KEY_DOWN) || App::IsKeyPressed(App::KEY_S))
    {
        if (m_stamina > 0.0f)
        {
            m_isSwimmingDown = true;
            ReduceStamina(15.0f * dtSec); 
           
            m_y = 220.0f; 
        }
        else 
        {
           
             m_y = 180.0f;
        }
    }
    else
    {
        
        m_y = 200.0f;
        RecoverStamina(10.0f * dtSec); 
    }

    
    m_sprite = m_facingRight ? m_spriteRight : m_spriteLeft;

    
    GameObject::Update(dt);

    
    if (!m_swarm.empty()) 
    {
        // --- PARÁMETROS PARA AJUSTAR EL MOVIMIENTO ---
        const float FORMATION_RADIUS = 60.0f;
        const float ROTATION_SPEED = 0.5f;
        const float PULSE_SPEED = 2.0f;
        const float PULSE_AMPLITUDE = 10.0f;

        
        m_swarmRotationAngle += ROTATION_SPEED * dtSec;
        m_swarmTime += dtSec;
        
        
        float dynamicRadius = FORMATION_RADIUS + (sinf(m_swarmTime * PULSE_SPEED) * PULSE_AMPLITUDE);

        const float M_PI = 3.1415926535f;

        for (size_t i = 0; i < m_swarm.size(); ++i)
        {
            float angle = m_swarmRotationAngle + (i / (float)m_swarm.size()) * 2.0f * M_PI;

            
            float offsetX = dynamicRadius * cosf(angle);
            float offsetY = dynamicRadius * sinf(angle);
            
            
            m_swarm[i]->UpdateEscortPosition(m_x + offsetX, m_y + offsetY, dt);
            
           
            m_swarm[i]->Update(dt);
        }
    }
    
}

void Player::Draw()
{
    GameObject::Draw(); 

    
    for (auto ally : m_swarm)
    {
        ally->Draw();
    }
}


bool Player::IsSwimmingDown() const
{
    return m_isSwimmingDown;
}

void Player::AddAllyMass(float amount)
{
    m_mass += amount;
}

void Player::ReduceStamina(float amount)
{
    m_stamina -= amount;
    if (m_stamina < 0.0f) m_stamina = 0.0f;
}

void Player::RecoverStamina(float amount)
{
    m_stamina += amount;
    if (m_stamina > 100.0f) m_stamina = 100.0f;
}

void Player::AddAllyToSwarm(CollectableAlly* ally)
{
    ally->SetState(AllyState::ESCORTE);
    ally->SetEscortOffset(0.0f, 0.0f); 
    m_swarm.push_back(ally);
}