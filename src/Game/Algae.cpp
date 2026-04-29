#include "Algae.h"
#include "GameConstants.h"
#include <math.h>

Algae::Algae(float startX, float startY)
    : GameObject("./data/TestData/alga.bmp", 1, 1), m_isActive(true)
{
    SetPosition(startX, startY);
	SetScale(0.25f);
    
    
    // Configuramos colores para que se vea verde (asumiendo que CSimpleSprite o el blend se pueda,
    // pero si no, simplemente lo dejamos con su grafico original, a 0.08x será visualmente distinto)
    
    m_speedY = -150.0f; // Flota suavemente hacia arriba
    m_oscillationTime = 0.0f;
}

Algae::~Algae()
{
}

void Algae::Update(float dt)
{
    if (!m_isActive) return;

    float dtSec = dt / 1000.0f;
    m_oscillationTime += dtSec;

    // Movimiento hacia arriba y ondulante
    m_y += m_speedY * dtSec;
    m_x += sinf(m_oscillationTime * 3.0f) * 1.5f; // Ondulación suave en X

    // Desactivar si sale mucho por arriba de la pantalla
    // Sumamos 200 al límite inferior porque las algas nacen por debajo de la pantalla en Y=700
    if (m_y < -50.0f || m_y > GameConfig::GAME_DESPAWN_Y + 200.0f)
    {
        m_isActive = false;
    }

    GameObject::Update(dt);
}

void Algae::Spawn(float startX, float startY)
{
    SetPosition(startX, startY);
    m_isActive = true;
    m_oscillationTime = 0.0f;
}
