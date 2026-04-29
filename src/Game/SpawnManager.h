#ifndef _SPAWN_MANAGER_H
#define _SPAWN_MANAGER_H

#include <vector>

// Forward declarations
class Obstacle;
class CollectableAlly;
class Player;

class SpawnManager
{
public:
    SpawnManager();
    ~SpawnManager();

    void Update(float dt, float netForce, float currentDepth);
    void Draw();
    void Reset();
    void CheckCollisions(Player* player);

private:
    std::vector<Obstacle*> m_activeObstacles;
    std::vector<Obstacle*> m_obstaclePool;
    float m_obstacleSpawnTimer;

    std::vector<CollectableAlly*> m_activeAllies;
    std::vector<CollectableAlly*> m_allyPool;
    float m_allySpawnTimer;
    
    std::vector<class Algae*> m_activeAlgae;
    std::vector<class Algae*> m_algaePool;
    float m_algaeSpawnTimer;
    bool m_spaceWasPressed;
};

#endif
