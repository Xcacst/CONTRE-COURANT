#include "SpawnManager.h"
#include "Obstacle.h"
#include "CollectableAlly.h"
#include "Player.h"
#include "GameConstants.h"
#include "UIManager.h"
#include "Algae.h"
#include "../ContestAPI/app.h"
#include <stdlib.h>
#include <cstdio>
#include <math.h>

extern UIManager* g_uiManager;

// Helper local para usar AABB en el manager
static bool IsCollidingObj(GameObject* a, GameObject* b)
{
    float ax, ay, bx, by;
    a->GetPosition(ax, ay);
    b->GetPosition(bx, by);

    float aw = a->GetWidth() * a->GetSprite()->GetScale();
    float ah = a->GetHeight() * a->GetSprite()->GetScale();
    float bw = b->GetWidth() * b->GetSprite()->GetScale();
    float bh = b->GetHeight() * b->GetSprite()->GetScale();

    return (abs(ax - bx) < (aw / 2.0f + bw / 2.0f - 10.0f)) &&
        (abs(ay - by) < (ah / 2.0f + bh / 2.0f - 10.0f));
}

SpawnManager::SpawnManager()
{
    m_obstacleSpawnTimer = 0.0f;
    m_allySpawnTimer = 0.0f;

    for (int i = 0; i < 10; ++i) {
        Obstacle* b = new Obstacle(0, 0, TRASH_PLASTIC_BAG); b->Deactivate(); m_obstaclePool.push_back(b);
        Obstacle* n = new Obstacle(0, 0, TRASH_NET);         n->Deactivate(); m_obstaclePool.push_back(n);
        Obstacle* c = new Obstacle(0, 0, TRASH_CAN);         c->Deactivate(); m_obstaclePool.push_back(c);
        Obstacle* bt = new Obstacle(0, 0, TRASH_BOTTLE);     bt->Deactivate(); m_obstaclePool.push_back(bt);
    }
    for (int i = 0; i < 30; ++i) {
        CollectableAlly* a = new CollectableAlly(0, 0, "./data/TestData/PEZ_DER.bmp", "./data/TestData/PEZ_IZQ.bmp");
        m_allyPool.push_back(a);
    }
    
    m_algaeSpawnTimer = 0.0f;
    m_spaceWasPressed = false;
    for (int i = 0; i < 20; ++i) {
        class Algae* a = new class Algae(0, 0); // Usaremos #include "Algae.h"
        a->SetActive(false);
        m_algaePool.push_back(a);
    }
}

SpawnManager::~SpawnManager()
{
    for (auto obs : m_activeObstacles) delete obs;
    for (auto obs : m_obstaclePool) delete obs;
    m_activeObstacles.clear();
    m_obstaclePool.clear();

    for (auto ally : m_activeAllies) delete ally;
    for (auto ally : m_allyPool) delete ally;
    m_activeAllies.clear();
    m_allyPool.clear();
    
    for (auto algae : m_activeAlgae) delete algae;
    for (auto algae : m_algaePool) delete algae;
    m_activeAlgae.clear();
    m_algaePool.clear();
}

void SpawnManager::Reset()
{
    for (auto obs : m_activeObstacles) {
        obs->Deactivate();
        m_obstaclePool.push_back(obs);
    }
    m_activeObstacles.clear();

    for (auto ally : m_activeAllies) {
        m_allyPool.push_back(ally);
    }
    m_activeAllies.clear();

    m_obstacleSpawnTimer = 0.0f;
    m_allySpawnTimer = 0.0f;
}

void SpawnManager::Update(float dt, float netForce, float currentDepth)
{
    float dtSec = dt / 1000.0f;

    // Calcular la dificultad basada en la profundidad (400m iniciales a 2300m máximo)
    float difficultyFactor = (currentDepth - GameConfig::START_DEPTH) / (GameConfig::WIN_DEPTH - GameConfig::START_DEPTH);
    if (difficultyFactor < 0.0f) difficultyFactor = 0.0f;
    if (difficultyFactor > 1.0f) difficultyFactor = 1.0f;

    // Escalar tiempos de aparición
    // Los obstáculos salen más rápido a mayor profundidad (de 2.0s a 0.8s)
    float obstacleSpawnThreshold = 2.0f - (1.2f * difficultyFactor);
    // Los aliados tardan más en salir a mayor profundidad (de 3.0s a 5.0s)
    float allySpawnThreshold = 3.0f + (2.0f * difficultyFactor);

    // --- OBSTACLES ---
    m_obstacleSpawnTimer += dtSec;
    if (m_obstacleSpawnTimer > obstacleSpawnThreshold)
    {
        m_obstacleSpawnTimer = 0.0f;
        float spawnX = GameConfig::GAME_MIN_X + (float)(rand() % (int)(GameConfig::GAME_MAX_X - GameConfig::GAME_MIN_X));
        float spawnY = GameConfig::SPAWN_START_Y;

        // Elegir el tipo de basura basado en la dificultad
        int r = rand() % 100;
        ObstacleType type;

        if (difficultyFactor > 0.6f) {
            // Si estamos muy profundo: 15% bolsa, 30% red, 30% lata, 25% botella
            if (r < 15)       type = TRASH_PLASTIC_BAG;
            else if (r < 45)  type = TRASH_NET;
            else if (r < 75)  type = TRASH_CAN;
            else              type = TRASH_BOTTLE;
        }
        else {
            // Si estamos cerca de la superficie: 40% bolsa, 30% red, 15% lata, 15% botella
            if (r < 40)       type = TRASH_PLASTIC_BAG;
            else if (r < 70)  type = TRASH_NET;
            else if (r < 85)  type = TRASH_CAN;
            else              type = TRASH_BOTTLE;
        }

        Obstacle* spawnObs = nullptr;
        if (!m_obstaclePool.empty()) {
            for (auto it = m_obstaclePool.begin(); it != m_obstaclePool.end(); ++it) {
                if ((*it)->GetType() == type) {
                    spawnObs = *it;
                    m_obstaclePool.erase(it);
                    break;
                }
            }
        }
        if (!spawnObs) spawnObs = new Obstacle(spawnX, spawnY, type);

        spawnObs->SetPosition(spawnX, spawnY);
        spawnObs->Activate();
        m_activeObstacles.push_back(spawnObs);
    }

    // Logic Obstacles
    for (size_t i = 0; i < m_activeObstacles.size(); i++)
    {
        Obstacle* obs = m_activeObstacles[i];
        if (!obs->IsActive()) continue;

        float relativeSpeed = netForce;
        float ox, oy;
        obs->GetPosition(ox, oy);

        oy += relativeSpeed * dtSec;

        // Aumentar la velocidad de caída de los obstáculos según la dificultad
        float extraFallSpeed = 30.0f + (60.0f * difficultyFactor); // Sube de 30 hasta 90
        oy += extraFallSpeed * dtSec;

        obs->SetPosition(ox, oy);
        obs->Update(dt);

        if (oy > GameConfig::DESPAWN_Y_TOP || oy < GameConfig::DESPAWN_Y_BOTTOM) obs->Deactivate();
    }

    // Clean Inactive Obstacles
    for (auto it = m_activeObstacles.begin(); it != m_activeObstacles.end(); )
    {
        Obstacle* obs = *it;
        if (!obs->IsActive())
        {
            m_obstaclePool.push_back(obs);
            it = m_activeObstacles.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // --- ALLIES ---
    m_allySpawnTimer += dtSec;
    if (m_allySpawnTimer > allySpawnThreshold)
    {
        m_allySpawnTimer = 0.0f;
        float spawnX = GameConfig::GAME_MIN_X + (float)(rand() % (int)(GameConfig::GAME_MAX_X - GameConfig::GAME_MIN_X));
        float spawnY = GameConfig::SPAWN_START_Y;
        CollectableAlly* newAlly = nullptr;
        if (!m_allyPool.empty()) {
            newAlly = m_allyPool.back();
            m_allyPool.pop_back();
        } else {
            newAlly = new CollectableAlly(0, 0, "./data/TestData/PEZ_DER.bmp", "./data/TestData/PEZ_IZQ.bmp");
        }
        newAlly->SetPosition(spawnX, spawnY);
        newAlly->SetState(AllyState::PASSING);
        m_activeAllies.push_back(newAlly);
    }

    // Logic Allies
    for (auto it = m_activeAllies.begin(); it != m_activeAllies.end(); )
    {
        CollectableAlly* ally = *it;
        float relativeSpeed = netForce;
        float ax, ay;
        ally->GetPosition(ax, ay);
        ay += relativeSpeed * dtSec;
        ally->SetPosition(ax, ay);
        ally->Update(dt);

        if (ay > GameConfig::DESPAWN_Y_TOP || ay < GameConfig::DESPAWN_Y_BOTTOM)
        {
            m_allyPool.push_back(ally);
            it = m_activeAllies.erase(it);
        }
        else ++it;
    }

    // Aplicar Boids a todos los aliados activos
    for (auto ally : m_activeAllies)
    {
        ally->ApplyBoids(m_activeAllies);
    }

    // --- ALGAE SPAWN & UPDATE ---
    m_algaeSpawnTimer += dtSec;
    if (m_algaeSpawnTimer > GameConfig::ALGAE_SPAWN_RATE_START)
    {
        m_algaeSpawnTimer = 0.0f;
        float ax = GameConfig::GAME_MIN_X + (float)(rand() % (int)(GameConfig::GAME_MAX_X - GameConfig::GAME_MIN_X));
        Algae* a = nullptr;
        if (!m_algaePool.empty()) {
            a = m_algaePool.back();
            m_algaePool.pop_back();
        } else {
            a = new Algae(0, 0);
        }
        // DESPAWN_Y_TOP (650) + 50 = 700 (abajo de la pantalla visible), para que suba flotando.
        a->Spawn(ax, GameConfig::DESPAWN_Y_TOP + 50.0f); 
        m_activeAlgae.push_back(a);
    }

    for (auto it = m_activeAlgae.begin(); it != m_activeAlgae.end();)
    {
        (*it)->Update(dt);
        if (!(*it)->IsActive())
        {
            m_algaePool.push_back(*it);
            it = m_activeAlgae.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void SpawnManager::CheckCollisions(Player* player)
{
    if (!player) return;

    for (auto it = m_activeAlgae.begin(); it != m_activeAlgae.end(); )
    {
        Algae* a = *it;
        if (IsCollidingObj(player, a))
        {
            player->AddAlgae(1);
            a->SetActive(false);
            
            float px, py;
            player->GetPosition(px, py);
            if (g_uiManager) g_uiManager->AddPopup(px, py + 30.0f, "+1 ALGAE", 0.0f, 1.0f, 0.0f);
            
            m_algaePool.push_back(a);
            it = m_activeAlgae.erase(it);
        }
        else
        {
            ++it;
        }
    }

    bool spacePressed = App::IsKeyPressed(App::KEY_SPACE);
    if (spacePressed && !m_spaceWasPressed)
    {
        if (player->GetAlgaeCount() >= GameConfig::ALGAE_COST_TO_CALL)
        {
            int alliesCalled = 0;
            float px, py;
            player->GetPosition(px, py);

            for (auto it = m_activeAllies.begin(); it != m_activeAllies.end(); )
            {
                if ((*it)->GetState() == AllyState::PASSING)
                {
                    bool added = player->AddAllyToSwarm(*it);
                    if (added)
                    {
                        player->AddAllyMass((*it)->GetMassValue());
                        alliesCalled++;
                        it = m_activeAllies.erase(it);
                        if (alliesCalled >= 2) break;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    ++it;
                }
            }

            if (alliesCalled > 0)
            {
                player->ReduceAlgae(GameConfig::ALGAE_COST_TO_CALL);
                if (g_uiManager) g_uiManager->AddPopup(px, py + 45.0f, "ALLIES CALLED", 0.0f, 0.8f, 1.0f);
            }
        }
    }
    m_spaceWasPressed = spacePressed;


    // Obstacles collision
    for (size_t i = 0; i < m_activeObstacles.size(); i++)
    {
        Obstacle* obs = m_activeObstacles[i];
        if (!obs->IsActive()) continue;

        if (IsCollidingObj(player, obs))
        {
            obs->Deactivate();

            float px, py;
            player->GetPosition(px, py);


            // Disparar flash rojo en cualquier colisión
            if (g_uiManager) g_uiManager->TriggerFlash();

            // --- MECÁNICA DE ESCUDO: si hay aliados, uno absorbe el golpe ---
            bool allyAbsorbed = player->SacrificeAlly();

            if (allyAbsorbed)
            {
                if (g_uiManager) g_uiManager->AddPopup(px, py + 30.0f, "ALLY LOST!", 1.0f, 0.6f, 0.0f);
            }
            else
            {
                char dmgText[32];
                if (obs->GetType() == TRASH_PLASTIC_BAG) {
                    player->ReduceStamina(GameConfig::PENALTY_PLASTIC_BAG);
                    sprintf(dmgText, "-%.0f STAMINA", GameConfig::PENALTY_PLASTIC_BAG);
                    if (g_uiManager) g_uiManager->AddPopup(px, py + 30.0f, dmgText, 1.0f, 0.3f, 0.3f);
                }
                else if (obs->GetType() == TRASH_NET) {
                    player->ReduceStamina(GameConfig::PENALTY_NET);
                    sprintf(dmgText, "-%.0f STAMINA", GameConfig::PENALTY_NET);
                    if (g_uiManager) g_uiManager->AddPopup(px, py + 30.0f, dmgText, 1.0f, 0.5f, 0.5f);
                }
                else if (obs->GetType() == TRASH_CAN) {
                    player->ReduceStamina(GameConfig::PENALTY_CAN);
                    sprintf(dmgText, "-%.0f STAMINA !!", GameConfig::PENALTY_CAN);
                    if (g_uiManager) g_uiManager->AddPopup(px, py + 30.0f, dmgText, 1.0f, 0.0f, 0.0f);
                }
                else if (obs->GetType() == TRASH_BOTTLE) {
                    player->ReduceStamina(GameConfig::PENALTY_BOTTLE);
                    sprintf(dmgText, "-%.0f STAMINA", GameConfig::PENALTY_BOTTLE);
                    if (g_uiManager) g_uiManager->AddPopup(px, py + 30.0f, dmgText, 1.0f, 0.4f, 0.0f);
                }
            }

        }
    }

    // Allies collision
    for (auto it = m_activeAllies.begin(); it != m_activeAllies.end(); )
    {
        CollectableAlly* ally = *it;
        if (IsCollidingObj(player, ally))
        {
            float px, py;
            player->GetPosition(px, py);

            bool added = player->AddAllyToSwarm(ally);
            if (added)
            {
                player->AddAllyMass(ally->GetMassValue());
                player->RecoverStamina(GameConfig::ALLY_STAMINA_HEAL);
                it = m_activeAllies.erase(it);

                char recText[32];
                sprintf(recText, "+%.0f STAMINA", GameConfig::ALLY_STAMINA_HEAL);
                if (g_uiManager) g_uiManager->AddPopup(px, py + 30.0f, recText, 0.3f, 1.0f, 0.3f);
            }
            else
            {
                if (g_uiManager) g_uiManager->AddPopup(px, py + 30.0f, "SWARM FULL!", 1.0f, 0.8f, 0.0f);
                ++it;
            }
        }
        else
        {
            ++it;
        }
    }
}

void SpawnManager::Draw()
{
    for (auto algae : m_activeAlgae)
    {
        if (algae->IsActive()) algae->Draw();
    }

    for (auto obs : m_activeObstacles)
    {
        if (obs->IsActive()) obs->Draw();
    }

    for (auto ally : m_activeAllies)
    {
        ally->Draw();
    }
}