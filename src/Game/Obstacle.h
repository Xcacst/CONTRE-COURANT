#ifndef _OBSTACLE_H
#define _OBSTACLE_H

#include "GameObject.h"

enum ObstacleType
{
    TRASH_PLASTIC_BAG,  // Reduce Stamina (Te asfixia)
    TRASH_NET,          // Reduce Masa (Atrapa a tus aliados)
    TRASH_BOTTLE,
	TRASH_CAN,
};

class Obstacle : public GameObject
{
public:
    Obstacle(float startX, float startY, ObstacleType type);
    virtual ~Obstacle();

    virtual void Update(float dt) override;
    
    ObstacleType GetType() const { return m_type; }
    bool IsActive() const { return m_active; }
    void Deactivate() { m_active = false; } // Para cuando sale de pantalla o choca
    void Activate() { m_active = true; }
private:
    ObstacleType m_type;
    float m_verticalSpeed; // Velocidad propia de hundimiento (o flotación)
    bool m_active;
};

#endif
