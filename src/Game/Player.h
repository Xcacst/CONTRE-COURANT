#ifndef _PLAYER_H
#define _PLAYER_H

#include "GameObject.h"
#include <vector> 

class CollectableAlly; 

class Player : public GameObject
{
public:
    Player();
    virtual ~Player();

    virtual void Update(float dt) override;
    virtual void Draw() override; 

    // Méthod Gameplay
    bool IsSwimmingDown() const;
    float GetMass() const { return m_mass; }
    float GetStamina() const { return m_stamina; }
    
    // Gestión state allys
    void AddAllyMass(float amount);
    void ReduceStamina(float amount);
    void RecoverStamina(float amount);
    
    int GetAlgaeCount() const { return m_algaeCount; }
    void AddAlgae(int amount);
    void ReduceAlgae(int amount);

    // gestion allys
    bool AddAllyToSwarm(CollectableAlly* ally);  
    bool SacrificeAlly();                        // Sacrifica un aliado para absorber daño
    int  GetSwarmSize() const { return (int)m_swarm.size(); }

private:
    CSimpleSprite* m_spriteRight;
    CSimpleSprite* m_spriteLeft;
    bool m_facingRight;

    float m_mass;
    float m_stamina;
    int m_algaeCount;
    float m_speedX;
    
    // Estado de input
    bool m_isSwimmingDown;

    std::vector<CollectableAlly*> m_swarm; 

    // --- VARIABLES PARA EL ENJAMBRE ---
    float m_swarmRotationAngle; 
    float m_swarmTime;          // pulse effect
};

#endif

