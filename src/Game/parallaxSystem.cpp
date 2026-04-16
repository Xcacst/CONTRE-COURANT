#include "ParallaxSystem.h"
#include "Player.h"
#include <cstdlib>
#include <math.h>

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

    // Spawneamos en ambas direcciones
    if (netForce != 0.0f) { // Or add a small epsilon if desired
        m_spawnTimer += dtSec;
        if (m_spawnTimer > 0.6f) { // Adjusted to spawn a bit faster since we have more layers
            // Si el objeto viaja hacia arriba en la pantalla (netForce > 0), spawnea abajo
            // Si el objeto viaja hacia abajo (netForce < 0), spawnea arriba
            float startY = (netForce > 0) ? -200.0f : 1050.0f;
            SpawnLayer(startY);
            m_spawnTimer = 0.0f;
        }
    }

    for (int i = 0; i < NUM_LAYERS; ++i) {
        if (!m_layers[i].isActive) continue;

        float x, y;
        m_layers[i].sprite->GetPosition(x, y);

        // Movimiento: Sube si netForce es +, baja si netForce es -
        y += (netForce * m_layers[i].speedMultiplier) * dtSec;

        // Limpieza de pantalla con márgenes fijos (800.0f para limpieza más ajustada a la cámara)
        if (y > 800.0f || y < -350.0f) {
            m_layers[i].isActive = false;
        }

        m_layers[i].sprite->SetPosition(x, y);
    }
}

void ParallaxSystem::SpawnLayer(float startY) {
    for (int i = 0; i < NUM_LAYERS; ++i) {
        if (!m_layers[i].isActive) {
            float depthFactor = 0.2f + (static_cast<float>(rand() % 60) / 100.0f); // 0.2f to 0.79f
            m_layers[i].speedMultiplier = depthFactor;

            // --- POSICIÓN RESTRINGIDA A LOS LADOS PARA GAMEPLAY ---
            // El usuario quiere que sean obstáculos físicos que estrechen el nivel
            float posX;
            // Alternamos lados y añadimos algo de margen local
            if (m_lastSideLeft) {
                posX = 880.0f + static_cast<float>(rand() % 70); // Borde Derecho
                m_lastSideLeft = false;
            }
            else {
                posX = static_cast<float>(rand() % 70);          // Borde Izquierdo
                m_lastSideLeft = true;
            }

            // Desfase en Y aleatorio
            float randomYOffset = static_cast<float>(rand() % 200) - 100.0f;

            m_layers[i].sprite->SetPosition(posX, startY + randomYOffset);

            // ESCALA
            m_layers[i].sprite->SetScale(depthFactor * 0.7f);

            // COLOR TINT: Entre más lejos, menos rojo y un poco menos verde (apariencia coral oscuro bajo el agua)
            // Cuando depthFactor es bajo (0.2)-> m_red será 0.2, m_green 0.3. Para depth altos (0.79), será cercano a blanco/color original
            m_layers[i].sprite->SetColor(depthFactor, depthFactor * 1.25f, 1.0f);

            m_layers[i].isActive = true;
            break;
        }
    }
}

void ParallaxSystem::Render() {
    // Ordenar índices para el Z-Sorting
    int activeIndices[NUM_LAYERS];
    int activeCount = 0;
    
    for (int i = 0; i < NUM_LAYERS; ++i) {
        if (m_layers[i].isActive && m_layers[i].sprite) {
            activeIndices[activeCount++] = i;
        }
    }

    // Ordenamiento burbuja por profundidad (de menor multiplicador al mayor)
    // Para que los más lejanos (menor Z) se pinten primero y queden DEBAJO
    for (int i = 0; i < activeCount - 1; ++i) {
        for (int j = 0; j < activeCount - i - 1; ++j) {
            if (m_layers[activeIndices[j]].speedMultiplier > m_layers[activeIndices[j + 1]].speedMultiplier) {
                int temp = activeIndices[j];
                activeIndices[j] = activeIndices[j + 1];
                activeIndices[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < activeCount; ++i) {
        m_layers[activeIndices[i]].sprite->Draw();
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

void ParallaxSystem::CheckCollisions(Player* player) {
    if (!player) return;

    float px, py;
    player->GetPosition(px, py);

    // Obtener las dimensiones del jugador escaladas
    float pw = player->GetWidth() * player->GetSprite()->GetScale();
    float ph = player->GetHeight() * player->GetSprite()->GetScale();

    // Encojemos el hitbox del jugador a un 60% para mayor tolerancia (esquinas invisibles)
    float pw_hitbox = pw * 0.6f;
    float ph_hitbox = ph * 0.5f; // Un poco más indulgente en la altura del pez

    for (int i = 0; i < NUM_LAYERS; ++i) {
        if (!m_layers[i].isActive || !m_layers[i].sprite) continue;

        float cx, cy;
        m_layers[i].sprite->GetPosition(cx, cy);

        float cw = m_layers[i].sprite->GetWidth() * m_layers[i].sprite->GetScale();
        float ch = m_layers[i].sprite->GetHeight() * m_layers[i].sprite->GetScale();

        // Encojemos el hitbox del coral también (ej. al 65% de la imagen original)
        float cw_hitbox = cw * 0.65f;
        float ch_hitbox = ch * 0.65f;

        // Chequeo de colisión física pura usando las cajas reducidas "hitboxes"
        if ((abs(px - cx) < (pw_hitbox / 2.0f + cw_hitbox / 2.0f)) &&
            (abs(py - cy) < (ph_hitbox / 2.0f + ch_hitbox / 2.0f))) 
        {
            // Resolución física en el Eje X (Bloqueo rígido)
            if (px > cx) {
                // Empujar a la derecha
                px = cx + (cw_hitbox / 2.0f + pw_hitbox / 2.0f);
            } else {
                // Empujar a la izquierda
                px = cx - (cw_hitbox / 2.0f + pw_hitbox / 2.0f);
            }
            player->SetPosition(px, py);
        }
    }
}