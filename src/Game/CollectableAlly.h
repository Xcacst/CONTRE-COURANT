#ifndef _COLLECTABLE_ALLY_H
#define _COLLECTABLE_ALLY_H

#include "GameObject.h"
#include <vector> 

// ALLY
enum class AllyState
{
    PASSING,   
    JOINING,   
    ESCORTE    
};

class CollectableAlly : public GameObject
{
public:
    
    CollectableAlly(float startX, float startY, const char* spriteRightFile, const char* spriteLeftFile);
    virtual ~CollectableAlly();

    virtual void Update(float dt) override;
    virtual void Draw() override;

    float GetMassValue() const { return m_massValue; }
    AllyState GetState() const { return m_state; }
    void SetState(AllyState newState) { m_state = newState; }

	// Función boids para la interacción entre aliados en el estado JOINING
    void ApplyBoids(const std::vector<CollectableAlly*>& allies);

    // Para el estado ESCORTE
    void SetEscortOffset(float offsetX, float offsetY);
    void UpdateEscortPosition(float playerX, float playerY, float dt);

private:
    CSimpleSprite* m_spriteRight; //sprite rigth
    CSimpleSprite* m_spriteLeft;  // Sprite left
    bool m_facingRight;           // bool state 

    AllyState m_state;
    float m_massValue;
    float m_speedX;              // speed
	float m_speedY;              // vertial speed
    float m_velocityX;           // spped real x (
	float m_velocityY;           // speed real y 

    
    float m_offsetX;
    float m_offsetY;

    
    void UpdatePassing(float dt);
    
    void UpdateEscorte(float dt);
};

#endif
