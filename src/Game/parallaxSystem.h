#ifndef PARALLAX_SYSTEM_H
#define PARALLAX_SYSTEM_H

#include "../ContestAPI/app.h"

struct ParallaxLayer {
    CSimpleSprite* sprite = nullptr;
    bool isActive = false;
    float speedMultiplier = 1.0f;
};

class Player;

class ParallaxSystem {
public:
    ParallaxSystem();
    ~ParallaxSystem(); 

    void Init();
    void Update(float netForce, float dt);
    void Render();
    void Shutdown();
    void Reset();
    void CheckCollisions(Player* player);

private:
    static const int NUM_LAYERS = 20;
    ParallaxLayer m_layers[NUM_LAYERS];

    float m_spawnTimer;
    bool m_lastSideLeft;

    void SpawnLayer(float startY);
};

#endif