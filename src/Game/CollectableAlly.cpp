#include "CollectableAlly.h"
#include "../ContestAPI/app.h" // For App::CreateSprite
#include <stdlib.h> // rand
#include <math.h>   // sinf

CollectableAlly::CollectableAlly(float startX, float startY, const char* spriteRightFile, const char* spriteLeftFile)
    : GameObject(spriteRightFile, 1, 1) // GameObject usará spriteRightFile inicialmente
{
    m_spriteRight = m_sprite; // El sprite base es el derecho
    m_spriteLeft = App::CreateSprite(spriteLeftFile, 1, 1); // Cargar el sprite izquierdo
    
    // Asegurarse que ambos sprites tengan la misma escala inicial
    m_spriteLeft->SetScale(GetSprite()->GetScale());

    SetPosition(startX, startY);
    SetScale(0.08f); // Escala para aliados

    m_massValue = 5.0f; // Cada aliado suma 5kg
    m_speedX = (float)(rand() % 100 - 50) / 100.0f; // Pequeño movimiento lateral aleatorio
    m_speedY = (float)(rand() % 50) / 100.0f + 0.1f; // Pequeño movimiento vertical
    
    m_state = AllyState::PASSING;
    m_facingRight = (m_speedX >= 0); // Mirar en la dirección de movimiento inicial
    m_offsetX = 0.0f;
    m_offsetY = 0.0f;
}

CollectableAlly::~CollectableAlly()
{
    // Gestión de memoria cuidadosa para evitar Double Free
    
    // 1. Borrar el sprite izquierdo si existe y es diferente del derecho (por seguridad)
    if (m_spriteLeft && m_spriteLeft != m_spriteRight)
    {
        delete m_spriteLeft;
        m_spriteLeft = nullptr;
    }

    // 2. Borrar el sprite derecho (que originalmente era m_sprite)
    if (m_spriteRight)
    {
        delete m_spriteRight;
        m_spriteRight = nullptr;
    }

    // 3. CRUCIAL: Desvincular m_sprite de la clase base para que su destructor
    // no intente borrar una memoria que acabamos de liberar.
    m_sprite = nullptr;
}

void CollectableAlly::Update(float dt)
{
    switch (m_state)
    {
    case AllyState::PASSING:
        UpdatePassing(dt);
        break;
    case AllyState::ESCORTE:
        UpdateEscorte(dt);
        break;
    // case AllyState::JOINING: // No implementado aún
    //     break;
    }
    
    // Actualizar la posición del sprite activo (GameObject::Update se encarga de esto)
    // Pero antes debemos asegurar que m_sprite del GameObject sea el correcto.
    m_sprite = m_facingRight ? m_spriteRight : m_spriteLeft;
    GameObject::Update(dt);
}

void CollectableAlly::Draw()
{
    // Usar el sprite activo (ya seleccionado en Update)
    GameObject::Draw();
}

void CollectableAlly::UpdatePassing(float dt)
{
    // Movimiento libre como antes
    float dtSec = dt / 1000.0f;

    // Oscilación lateral
    static float time = 0.0f;
    time += dtSec;
    
    float newX = m_x + (sinf(time * 3.0f + m_x) * m_speedX * dtSec * 100.0f);
    m_x = newX;

    // Cambiar de dirección si se acerca a los bordes
    if (m_x < 50.0f && !m_facingRight)
    {
        m_facingRight = true;
    }
    else if (m_x > 974.0f && m_facingRight)
    {
        m_facingRight = false;
    }
    
    // El movimiento vertical se gestiona en GameTest.cpp con el netForce
    // Aquí solo el movimiento propio
}

void CollectableAlly::UpdateEscorte(float dt)
{
    // No hace nada por sí mismo, su posición es gestionada por el jugador
    // en UpdateEscortPosition. Aquí solo quizás alguna animación local
    // o rotación si fuera el caso.
}

void CollectableAlly::SetEscortOffset(float offsetX, float offsetY)
{
    m_offsetX = offsetX;
    m_offsetY = offsetY;
}

void CollectableAlly::UpdateEscortPosition(float playerX, float playerY, float dt)
{
    // Posicionar el aliado relativo al jugador
    float targetX = playerX + m_offsetX;
    float targetY = playerY + m_offsetY;

    // Para una transición suave, podemos interpolar
    float lerpFactor = 0.1f; // Ajusta para mayor o menor suavidad
    m_x = m_x + (targetX - m_x) * lerpFactor;
    m_y = m_y + (targetY - m_y) * lerpFactor;

    // Determinar dirección de sprite basado en la posición del jugador
    // Si el aliado está a la derecha del jugador, mira a la izquierda para "mirar" al jugador
    // Si el aliado está a la izquierda del jugador, mira a la derecha
    m_facingRight = (m_x < playerX);
}
