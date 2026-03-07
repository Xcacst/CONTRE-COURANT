#ifndef _COLLECTABLE_ALLY_H
#define _COLLECTABLE_ALLY_H

#include "GameObject.h"
#include <vector> // Necesario para la posición del enjambre

// Estados para los peces aliados
enum class AllyState
{
    PASSING,   // Pez nadando libremente
    JOINING,   // Pez uniéndose al enjambre (transición)
    ESCORTE    // Pez parte del enjambre, siguiendo al jugador
};

class CollectableAlly : public GameObject
{
public:
    // Constructor modificado para tomar ambos sprites
    CollectableAlly(float startX, float startY, const char* spriteRightFile, const char* spriteLeftFile);
    virtual ~CollectableAlly();

    virtual void Update(float dt) override;
    virtual void Draw() override; // Override Draw para elegir sprite

    float GetMassValue() const { return m_massValue; }
    AllyState GetState() const { return m_state; }
    void SetState(AllyState newState) { m_state = newState; }

    // Para el estado ESCORTE
    void SetEscortOffset(float offsetX, float offsetY);
    void UpdateEscortPosition(float playerX, float playerY, float dt);

private:
    CSimpleSprite* m_spriteRight; // Sprite para cuando mira a la derecha
    CSimpleSprite* m_spriteLeft;  // Sprite para cuando mira a la izquierda
    bool m_facingRight;           // Dirección actual del pez

    AllyState m_state;
    float m_massValue;
    float m_speedX;              // Velocidad lateral (cuando está PASSING)
    float m_speedY;              // Velocidad vertical (cuando está PASSING)

    // Para el modo ESCORTE: posición relativa al jugador
    float m_offsetX;
    float m_offsetY;

    // Métodos privados para el movimiento de cada estado
    void UpdatePassing(float dt);
    // void UpdateJoining(float dt); // Podríamos añadirlo si necesitamos una animación de unión
    void UpdateEscorte(float dt);
};

#endif
