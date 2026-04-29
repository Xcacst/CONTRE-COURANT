#include "Player.h"
#include "../ContestAPI/app.h"
#include "CollectableAlly.h" 
#include "GameConstants.h"
#include <algorithm> 
#include <math.h>


Player::Player() : GameObject("./data/TestData/PEZ_DER.bmp", 1, 1)
{
    // Cargar sprites direccionales
    m_spriteRight = m_sprite; // El inicial es el derecho
    m_spriteLeft = App::CreateSprite("./data/TestData/PEZ_IZQ.bmp", 1, 1);
    
    m_facingRight = true;

    m_mass = GameConfig::PLAYER_START_MASS;
    m_stamina = GameConfig::PLAYER_START_STAMINA;
    m_algaeCount = 0;
    m_speedX = GameConfig::PLAYER_SPEED_X;


    m_isSwimmingDown = false;
    
    m_swarmRotationAngle = 0.0f;
    m_swarmTime = 0.0f;

    SetScale(0.1f);
    SetPosition(GameConfig::SCREEN_CENTER_X, GameConfig::PLAYER_START_Y); 
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

    
    // Velocidad horizontal escalada inversamente por masa
    // (más aliados = más torpeza lateral)
    float effectiveSpeedX = m_speedX * (GameConfig::PLAYER_START_MASS / m_mass);
    if (effectiveSpeedX < GameConfig::PLAYER_MIN_SPEED_X)
        effectiveSpeedX = GameConfig::PLAYER_MIN_SPEED_X;

    if (App::IsKeyPressed(App::KEY_LEFT) || App::IsKeyPressed(App::KEY_A))
    {
        m_x -= effectiveSpeedX * dtSec;
        m_facingRight = false; 
    }
    if (App::IsKeyPressed(App::KEY_RIGHT) || App::IsKeyPressed(App::KEY_D))
    {
        m_x += effectiveSpeedX * dtSec;
        m_facingRight = true; 
    }

   
    if (m_x < GameConfig::GAME_MIN_X) m_x = GameConfig::GAME_MIN_X;
    if (m_x > GameConfig::GAME_MAX_X) m_x = GameConfig::GAME_MAX_X;

    // 2. Mecánica (Key DOWN)
    if (App::IsKeyPressed(App::KEY_DOWN) || App::IsKeyPressed(App::KEY_S))
    {
        if (m_stamina > 0.0f)
        {
            m_isSwimmingDown = true;
            ReduceStamina(GameConfig::PLAYER_DIVE_STAMINA_COST * dtSec); 
           
            m_y = GameConfig::PLAYER_DIVE_Y; 
        }
        else 
        {
           
             m_y = GameConfig::PLAYER_TIRED_Y;
        }
    }
    else
    {
        
        m_y = GameConfig::PLAYER_START_Y;
        RecoverStamina(GameConfig::PLAYER_IDLE_STAMINA_REGEN * dtSec); 
    }

    
    m_sprite = m_facingRight ? m_spriteRight : m_spriteLeft;

    
    GameObject::Update(dt);

    
    if (!m_swarm.empty()) 
    {
        m_swarmRotationAngle += GameConfig::SWARM_ROTATION_SPEED * dtSec;
        m_swarmTime += dtSec;
        
        
        float dynamicRadius = GameConfig::SWARM_FORMATION_RADIUS + (sinf(m_swarmTime * GameConfig::SWARM_PULSE_SPEED) * GameConfig::SWARM_PULSE_AMPLITUDE);

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
    if (m_stamina > GameConfig::PLAYER_START_STAMINA) m_stamina = GameConfig::PLAYER_START_STAMINA;
}

void Player::AddAlgae(int amount)
{
    m_algaeCount += amount;
}

void Player::ReduceAlgae(int amount)
{
    m_algaeCount -= amount;
    if (m_algaeCount < 0) m_algaeCount = 0;
}

bool Player::AddAllyToSwarm(CollectableAlly* ally)
{
    if ((int)m_swarm.size() >= GameConfig::SWARM_MAX_SIZE)
    {
        return false; // Swarm lleno
    }
    ally->SetState(AllyState::ESCORTE);
    ally->SetEscortOffset(0.0f, 0.0f); 
    m_swarm.push_back(ally);
    return true;
}

bool Player::SacrificeAlly()
{
    if (m_swarm.empty()) return false;

    // Sacar el último aliado del enjambre
    CollectableAlly* sacrificed = m_swarm.back();
    m_swarm.pop_back();

    // Su masa deja de contar
    m_mass -= GameConfig::ALLY_MASS_VALUE;
    if (m_mass < GameConfig::PLAYER_START_MASS) m_mass = GameConfig::PLAYER_START_MASS;

    delete sacrificed;
    return true;
}