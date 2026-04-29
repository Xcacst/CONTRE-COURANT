#ifndef _ALGAE_H
#define _ALGAE_H

#include "GameObject.h"

class Algae : public GameObject
{
public:
    Algae(float startX, float startY);
    virtual ~Algae();

    virtual void Update(float dt) override;

    bool IsActive() const { return m_isActive; }
    void SetActive(bool active) { m_isActive = active; }

    // Re-configurar cuando se recicla del pool
    void Spawn(float startX, float startY);

private:
    bool m_isActive;
    float m_speedY;
    float m_oscillationTime; 
};

#endif
