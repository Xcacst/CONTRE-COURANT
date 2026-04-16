#include "Obstacle.h"
#include "GameConstants.h"
#include <stdlib.h> 
#include <math.h>

static const char* ObstacleTypeToSprite(ObstacleType type)
{
    switch (type)
    {
    case TRASH_PLASTIC_BAG:
        return "./data/TestData/pneu.bmp";
    case TRASH_NET:
        return "./data/TestData/net.bmp";
    case TRASH_BOTTLE:
        return "./data/TestData/bottle.bmp";
    case TRASH_CAN: 
        return "./data/TestData/lata.bmp";
    default:
        return "./data/TestData/lata.bmp";
    }
}

Obstacle::Obstacle(float startX, float startY, ObstacleType type)
    : GameObject(ObstacleTypeToSprite(type), 1, 1), m_type(type)
{
    SetPosition(startX, startY);
    m_active = true;

    // CONFIG
    switch (m_type)
    {
    case TRASH_PLASTIC_BAG:
        SetScale(GameConfig::OBSTACLE_SCALE_BAG);
        m_verticalSpeed = GameConfig::OBSTACLE_SPEED_BAG; 
        break;
    case TRASH_NET:
        SetScale(GameConfig::OBSTACLE_SCALE_NET); 
        m_verticalSpeed = GameConfig::OBSTACLE_SPEED_NET;
        break;
    case TRASH_BOTTLE:
        SetScale(GameConfig::OBSTACLE_SCALE_BOTTLE);
        m_verticalSpeed = GameConfig::OBSTACLE_SPEED_BOTTLE;
        break;
    case TRASH_CAN: 
        SetScale(GameConfig::OBSTACLE_SCALE_CAN);
        m_verticalSpeed = GameConfig::OBSTACLE_SPEED_CAN; 
        break;
    default:
        SetScale(GameConfig::OBSTACLE_SCALE_DEFAULT);
        m_verticalSpeed = GameConfig::OBSTACLE_SPEED_DEFAULT;
        break;
    }
}


Obstacle::~Obstacle()
{
    
}

void Obstacle::Update(float dt)
{
    if (!m_active) return;
    float dtSec = dt / 1000.0f;

    
    if (m_type == TRASH_NET) {

        static float zigTimer = 0.0f;
        zigTimer += dtSec;
        m_x += sinf(zigTimer * 5.0f) * 4.0f;
    }
    else if (m_type == TRASH_BOTTLE) {
        // Deriva lenta y suave de lado a lado (simula corriente)
        static float driftTimer = 0.0f;
        driftTimer += dtSec;
        m_x += sinf(driftTimer * 1.5f) * 1.5f;
    }
    else if (m_type == TRASH_CAN) {
        
        m_x += (float)((rand() % 5) - 2);
    }

   
    m_y += m_verticalSpeed * dtSec;

    GameObject::Update(dt);
}