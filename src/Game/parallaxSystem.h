#ifndef PARALLAX_SYSTEM_H
#define PARALLAX_SYSTEM_H

#include "../ContestAPI/app.h"

struct ParallaxLayer {
    CSimpleSprite* sprite = nullptr;
    bool isActive = false;
    float speedMultiplier = 1.0f;
};

class ParallaxSystem {
public:
    ParallaxSystem();
    ~ParallaxSystem(); // El destructor debe estar declarado aquí

    void Init();
    void Update(float netForce, float dt);
    void Render();
    void Shutdown();
    void Reset();

private:
    static const int NUM_LAYERS = 15;
    ParallaxLayer m_layers[NUM_LAYERS];

    float m_spawnTimer;
    bool m_lastSideLeft;

    void SpawnLayer(); // Esta es la función que te marcaba error
};

#endif