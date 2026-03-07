#include "Obstacle.h"
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
    case TRASH_CAN: // Asegúrate de tener este tipo en tu Enum
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

    // CONFIGURACIÓN DE DIFICULTAD [cite: 661]
    switch (m_type)
    {
    case TRASH_PLASTIC_BAG:
        SetScale(0.20f);
        m_verticalSpeed = 30.0f; // El neumático es lento pero pesado
        break;
    case TRASH_NET:
        SetScale(0.35f); // Hacemos la red más grande
        m_verticalSpeed = 25.0f;
        break;
    case TRASH_CAN: // LA LATA: Pequeña y muy veloz
        SetScale(0.12f);
        m_verticalSpeed = 120.0f; // ¡4 veces más rápida!
        break;
    default:
        SetScale(0.15f);
        m_verticalSpeed = 60.0f;
        break;
    }
}

// SOLUCIÓN AL ERROR LNK2019: Definición del destructor 
Obstacle::~Obstacle()
{
    // El cuerpo debe existir aunque esté vacío para que el Linker lo encuentre
}

void Obstacle::Update(float dt)
{
    if (!m_active) return;
    float dtSec = dt / 1000.0f;

    // COMPORTAMIENTOS ÚNICOS PARA SUBIR DIFICULTAD
    if (m_type == TRASH_NET) {
        // Zig-Zag agresivo [cite: 692]
        static float zigTimer = 0.0f;
        zigTimer += dtSec;
        m_x += sinf(zigTimer * 5.0f) * 4.0f;
    }
    else if (m_type == TRASH_CAN) {
        // La lata cae "girando" (vibración rápida)
        m_x += (float)((rand() % 5) - 2);
    }

    // Movimiento vertical propio [cite: 686]
    m_y += m_verticalSpeed * dtSec;

    GameObject::Update(dt);
}