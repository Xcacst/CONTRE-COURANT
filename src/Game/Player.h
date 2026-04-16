#ifndef _PLAYER_H
#define _PLAYER_H

#include "GameObject.h"
#include <vector> // Required for std::vector

class CollectableAlly; // Forward declaration to avoid circular include

class Player : public GameObject
{
public:
    Player();
    virtual ~Player();

    virtual void Update(float dt) override;
    virtual void Draw() override; // Override Draw to draw swarm

    // Métodos de Gameplay
    bool IsSwimmingDown() const;
    float GetMass() const { return m_mass; }
    float GetStamina() const { return m_stamina; }
    
    // Gestión de estado
    void AddAllyMass(float amount);
    void ReduceStamina(float amount);
    void RecoverStamina(float amount);

    // Para gestionar el enjambre
    bool AddAllyToSwarm(CollectableAlly* ally);  // Retorna false si el swarm está lleno
    bool SacrificeAlly();                        // Sacrifica un aliado para absorber daño
    int  GetSwarmSize() const { return (int)m_swarm.size(); }

private:
    CSimpleSprite* m_spriteRight;
    CSimpleSprite* m_spriteLeft;
    bool m_facingRight;

    float m_mass;
    float m_stamina;
    float m_speedX;
    
    // Estado de input
    bool m_isSwimmingDown;

    std::vector<CollectableAlly*> m_swarm; // El enjambre de aliados que siguen al jugador

    // --- VARIABLES PARA EL ENJAMBRE ---
    float m_swarmRotationAngle; // Guarda el ángulo de rotación actual del enjambre
    float m_swarmTime;          // Un contador de tiempo para el efecto de pulsación
};

#endif

