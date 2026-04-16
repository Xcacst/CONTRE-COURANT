#include "UIManager.h"
#include "../ContestAPI/app.h"
#include "Player.h"
#include "GameConstants.h"
#include <cstdio>
#include <algorithm>
#include <math.h>

UIManager::UIManager()
{
    m_highScore = GameConfig::LOSE_DEPTH;
    m_flashTimer = 0.0f;
}

UIManager::~UIManager()
{
    m_popups.clear();
}

void UIManager::Update(float dt)
{
    float dtSec = dt / 1000.0f;

    // Decrementar flash timer
    if (m_flashTimer > 0.0f)
    {
        m_flashTimer -= dtSec;
        if (m_flashTimer < 0.0f) m_flashTimer = 0.0f;
    }

    // Actualiza textos flotantes
    for (auto it = m_popups.begin(); it != m_popups.end(); )
    {
        it->lifeTime -= dtSec;
        it->y += 50.0f * dtSec; // El texto sube a 50 pixeles por segundo
        if (it->lifeTime <= 0.0f)
        {
            it = m_popups.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void UIManager::RenderVignette(Player* player, float currentDepth)
{
    float factor = (currentDepth - GameConfig::START_DEPTH) / (GameConfig::WIN_DEPTH - GameConfig::START_DEPTH);
    if (factor < 0.0f) factor = 0.0f;
    if (factor > 1.0f) factor = 1.0f;

    float maxAlpha = factor * 0.85f; // Oscuridad máxima de 85%

    if (maxAlpha <= 0.0f) return;

    // Habilitar mezcla alpha para transparencia
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float px = 0.0f;
    float py = 0.0f;
    if (player)
    {
        player->GetPosition(px, py);
        // Convert virtual coordinates (1024x768) to NDC (-1 to 1) 
        px = (px / 1024.0f) * 2.0f - 1.0f;
        py = (py / 768.0f) * 2.0f - 1.0f;
    }

    // Centro iluminado hacia oscuridad (Triangle Fan)
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f); // Centro transparente (luz)
    glVertex2f(px, py);

    glColor4f(0.0f, 0.0f, 0.0f, maxAlpha); // Borde oscuro
    const int numSegments = 24;
    const float R_OUT = 0.3f; // Radio donde termina la luz y empieza la oscuridad
    const float R_MAX = 3.0f; // Hasta donde tapamos la pantalla
    const float TWO_PI = 6.28318530718f;

    for (int i = 0; i <= numSegments; ++i)
    {
        float angle = i * TWO_PI / numSegments;
        float x = px + cosf(angle) * (R_OUT * (1024.0f / 768.0f)); 
        float y = py + sinf(angle) * R_OUT;
        glVertex2f(x, y);
    }
    glEnd();

    // Oscuridad total hacia el infinito (Triangle Strip) para tapar el resto de la pantalla
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= numSegments; ++i)
    {
        float angle = i * TWO_PI / numSegments;
        
        // Vértice interno (conecta con el abanico anterior)
        float xIn = px + cosf(angle) * (R_OUT * (1024.0f / 768.0f));
        float yIn = py + sinf(angle) * R_OUT;
        glVertex2f(xIn, yIn);
        
        // Vértice externo (cubre toda la pantalla)
        float xOut = px + cosf(angle) * (R_MAX * (1024.0f / 768.0f));
        float yOut = py + sinf(angle) * R_MAX;
        glVertex2f(xOut, yOut);
    }
    glEnd();

    // Deshabilitar para no afectar otros renders nativos
    glDisable(GL_BLEND);
}

void UIManager::TriggerFlash()
{
    m_flashTimer = GameConfig::FLASH_DURATION;
}

void UIManager::RenderFlash()
{
    if (m_flashTimer <= 0.0f) return;

    // Alpha proporcional al tiempo restante (fade out)
    float alpha = (m_flashTimer / GameConfig::FLASH_DURATION) * GameConfig::FLASH_MAX_ALPHA;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glColor4f(1.0f, 0.0f, 0.0f, alpha); // Rojo puro
    glVertex2f(-1.0f, -1.0f);
    glVertex2f( 1.0f, -1.0f);
    glVertex2f( 1.0f,  1.0f);
    glVertex2f(-1.0f,  1.0f);
    glEnd();

    glDisable(GL_BLEND);
}

void UIManager::RenderDepthBar(float currentDepth, float maxDepth)
{
    float barX      = GameConfig::DEPTH_BAR_X;
    float topY      = GameConfig::DEPTH_BAR_TOP_Y;
    float bottomY   = GameConfig::DEPTH_BAR_BOTTOM_Y;
    float barW      = GameConfig::DEPTH_BAR_WIDTH;
    float barHeight = topY - bottomY;

    // Progreso normalizado (0 = superficie/barco, 1 = meta/profundidad)
    float progress = (currentDepth - GameConfig::START_DEPTH) / (maxDepth - GameConfig::START_DEPTH);
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    // El relleno CAE desde arriba hacia abajo (más profundo = más relleno desde arriba)
    float fillHeight = barHeight * progress;
    float fillTop    = topY;           // empieza en la superficie (arriba)
    float fillBottom = topY - fillHeight; // cae hacia la meta (abajo)

    // --- Fondo de la barra (gris oscuro = agua sin explorar) ---
    for (float w = 0; w < barW; ++w)
    {
        App::DrawLine(barX + w, bottomY, barX + w, topY, 0.15f, 0.15f, 0.20f);
    }

    // --- Relleno (azul oscuro -> cian -> verde oscuro según profundidad) ---
    float r = 0.0f;
    float g = 0.2f + 0.5f * progress;  // Se vuelve más verde en profundidad
    float b = 0.9f - 0.4f * progress;  // Menos azul brillante en profundidad

    for (float w = 0; w < barW; ++w)
    {
        App::DrawLine(barX + w, fillBottom, barX + w, fillTop, r, g, b);
    }

    // --- Indicador del jugador (línea blanca, cae hacia abajo) ---
    float indicatorY = fillBottom;
    for (float w = -4; w < barW + 4; ++w)
    {
        App::DrawLine(barX + w, indicatorY, barX + w, indicatorY + 2.0f, 1.0f, 1.0f, 1.0f);
    }

    // --- Etiquetas: BARCO arriba, META abajo ---
    App::Print((int)(barX - 14), (int)(topY + 10),     "BARCO", 1.0f, 0.4f, 0.4f);
    App::Print((int)(barX - 10), (int)(bottomY - 18),  "META",  0.0f, 1.0f, 0.5f);
}

void UIManager::RenderHUD(Player* player, float currentDepth, float maxDepth)
{
    if (!player) return;

    // 1. Textos flotantes
    for (const auto& popup : m_popups)
    {
        // Calcular alpha/opacidad (como la API no soporta alpha, lo oscurecemos)
        float fade = popup.lifeTime / popup.maxLifeTime; 
        App::Print(popup.x, popup.y, popup.text.c_str(), popup.r * fade, popup.g * fade, popup.b * fade);
    }

    char buffer[128];
    
    // 2. Contador de aliados en el enjambre
    int swarmSize = player->GetSwarmSize();
    sprintf(buffer, "SWARM: %d / %d", swarmSize, GameConfig::SWARM_MAX_SIZE);
    float swarmR = (swarmSize > 0) ? 0.3f : 0.6f;
    float swarmG = (swarmSize > 0) ? 1.0f : 0.6f;
    float swarmB = (swarmSize > 0) ? 0.5f : 0.6f;
    App::Print(20, 740, buffer, swarmR, swarmG, swarmB);


    // 3. Mediciones del Jugador
    float stamina = player->GetStamina();
    float maxStamina = GameConfig::PLAYER_START_STAMINA;
    float tensionFactor = 1.0f - (stamina / maxStamina); 

    sprintf(buffer, "STAMINA: %.0f%%", stamina);
    App::Print(20, 720, buffer, tensionFactor, 1.0f - tensionFactor, 0.0f);

    // Agilidad: qué tan rápido te mueves horizontal (inverso a la masa)
    float effectiveSpeedX = GameConfig::PLAYER_SPEED_X * (GameConfig::PLAYER_START_MASS / player->GetMass());
    if (effectiveSpeedX < GameConfig::PLAYER_MIN_SPEED_X) effectiveSpeedX = GameConfig::PLAYER_MIN_SPEED_X;
    float agilityPct = effectiveSpeedX / GameConfig::PLAYER_SPEED_X * 100.0f;
    float agilityLoss = 1.0f - (agilityPct / 100.0f); // 0=ágil, 1=torpe
    sprintf(buffer, "AGILITY: %.0f%%", agilityPct);
    App::Print(20, 700, buffer, 0.3f + 0.7f * agilityLoss, 1.0f - 0.7f * agilityLoss, 0.0f);


    // 4. Barra visual gráfica de Stamina
    float barWidth = 200.0f;
    float fillWidth = barWidth * (stamina / maxStamina);
    // Fondo de la barra gris dibujando múltiples líneas para grosor visual
    for (int line = 0; line < 5; ++line) {
        App::DrawLine(20, 715 - line, 20 + barWidth, 715 - line, 0.3f, 0.3f, 0.3f); 
    }
    // Relleno de la barra (Verde -> Amarillo -> Rojo según la estamina)
    for (int line = 0; line < 5; ++line) {
        App::DrawLine(20, 715 - line, 20 + fillWidth, 715 - line, tensionFactor, 1.0f - tensionFactor, 0.0f); 
    }
}
 
void UIManager::RenderMenu()
{
    App::Print(300, 400, "", 1.0f, 1.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24);
    App::Print(320, 350, "", 1.0f, 1.0f, 1.0f);
    App::Print(320, 320, "", 0.8f, 0.8f, 0.8f);

    if (m_highScore > GameConfig::LOSE_DEPTH)
    {
        char buffer[128];
        sprintf(buffer, "Record Actual: %.1f m", m_highScore / 10.0f);
        App::Print(320, 250, buffer, 0.0f, 1.0f, 1.0f); // Color agua o cian
    }
}

void UIManager::RenderWon()
{
    App::Print(350, 400, "VICTORY!", 0.0f, 1.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24);
    App::Print(300, 350, "The boat has been sunk!", 1.0f, 1.0f, 1.0f);
    App::Print(320, 300, "Press R to Restart", 0.8f, 0.8f, 0.8f);
}

void UIManager::RenderLost()
{
    App::Print(350, 400, "GAME OVER", 1.0f, 0.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24);
    App::Print(300, 350, "You were caught!", 1.0f, 1.0f, 1.0f);
    
    char buffer[128];
    sprintf(buffer, "Llegaste a %.1f m de profundidad", m_highScore / 10.0f);
    App::Print(280, 320, buffer, 1.0f, 1.0f, 0.0f);

    App::Print(320, 280, "Press R to Restart", 0.8f, 0.8f, 0.8f);
}

void UIManager::AddPopup(float x, float y, const char* text, float r, float g, float b)
{
    FloatingText ft;
    ft.x = x;
    ft.y = y;
    ft.text = text;
    ft.r = r;
    ft.g = g;
    ft.b = b;
    ft.lifeTime = 1.5f;
    ft.maxLifeTime = 1.5f;
    m_popups.push_back(ft);
}

void UIManager::SetHighScore(float score)
{
    if (score > m_highScore)
    {
        m_highScore = score;
    }
}

float UIManager::GetHighScore() const
{
    return m_highScore;
}
