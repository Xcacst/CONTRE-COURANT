#include "ParallaxSystem.h"
#include <cstdlib>

ParallaxSystem::ParallaxSystem() : m_spawnTimer(0.0f), m_lastSideLeft(false) {
    for (int i = 0; i < NUM_LAYERS; ++i) {
        m_layers[i].sprite = nullptr;
        m_layers[i].isActive = false;
    }
}

ParallaxSystem::~ParallaxSystem() {
    Shutdown();
}

void ParallaxSystem::Init() {
    for (int i = 0; i < NUM_LAYERS; ++i) {
        m_layers[i].sprite = App::CreateSprite("./data/TestData/coral.bmp", 1, 1);
        m_layers[i].isActive = false;
    }
}

void ParallaxSystem::Update(float netForce, float dt) {
    float dtSec = dt / 1000.0f;

    // Spawneamos solo si bajamos (netForce > 0)
    // Ajusté el timer a 2.0f para que no salgan demasiadas y se vea más limpio
    if (netForce > 0) {
        m_spawnTimer += dtSec;
        if (m_spawnTimer > 1.0f) {
            SpawnLayer();
            m_spawnTimer = 0;
        }
    }

    for (int i = 0; i < NUM_LAYERS; ++i) {
        if (!m_layers[i].isActive) continue;

        float x, y;
        m_layers[i].sprite->GetPosition(x, y);

        // Movimiento: Sube si netForce es +, baja si netForce es -
        y += (netForce * m_layers[i].speedMultiplier * 1.0f) * dtSec;

        // Limpieza de pantalla
        if (y > 950.0f || y < -250.0f) {
            m_layers[i].isActive = false;
        }

        m_layers[i].sprite->SetPosition(x, y);
    }
}

void ParallaxSystem::SpawnLayer() {
    for (int i = 0; i < NUM_LAYERS; ++i) {
        if (!m_layers[i].isActive) {
            float depthFactor = 0.2f + (static_cast<float>(rand() % 60) / 100.0f);
            m_layers[i].speedMultiplier = depthFactor;

            // --- ALTERNAR LADOS SIN EMPALMES ---
            float posX;
            if (m_lastSideLeft) {
                posX = 880.0f + static_cast<float>(rand() % 70); // Derecha
                m_lastSideLeft = false;
            }
            else {
                posX = static_cast<float>(rand() % 70);          // Izquierda
                m_lastSideLeft = true;
            }

            // Desfase en Y para que no parezcan un desfile militar
            float randomYOffset = static_cast<float>(rand() % 200);

            m_layers[i].sprite->SetPosition(posX, -150.0f - randomYOffset);

            // ESCALA: La dejamos normal, sin rotaciones feas
            m_layers[i].sprite->SetScale(depthFactor * 0.7f);

            // quitamos la línea de SetAngle que lo arruinó todo

            m_layers[i].isActive = true;
            break;
        }
    }
}

void ParallaxSystem::Render() {
    for (int i = 0; i < NUM_LAYERS; ++i) {
        if (m_layers[i].isActive && m_layers[i].sprite) {
            m_layers[i].sprite->Draw();
        }
    }
}

void ParallaxSystem::Reset() {
    m_spawnTimer = 0.0f;
    for (int i = 0; i < NUM_LAYERS; ++i) {
        m_layers[i].isActive = false; // "Apagamos" todas las piedras
    }
}

void ParallaxSystem::Shutdown() {
    for (int i = 0; i < NUM_LAYERS; ++i) {
        if (m_layers[i].sprite) {
            delete m_layers[i].sprite;
            m_layers[i].sprite = nullptr;
        }
    }
}