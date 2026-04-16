#ifndef _UI_MANAGER_H
#define _UI_MANAGER_H

#include <vector>
#include <string>

class Player;

struct FloatingText {
    float x;
    float y;
    std::string text;
    float r, g, b;
    float lifeTime;     // Tiempo de vida restante
    float maxLifeTime;  // Utilizado para efecto de desvanecimiento
};

class UIManager
{
public:
    UIManager();
    ~UIManager();

    // Actualiza lógicas visuales (textos flotantes subiendo y desvaneciéndose)
    void Update(float dt);

    // Dibuja el HUD durante el juego
    void RenderHUD(Player* player, float currentDepth, float maxDepth);

    // Oscurece la pantalla gradualmente según la profundidad con Spotlight en el jugador
    void RenderVignette(Player* player, float currentDepth);

    // Flash rojo al recibir daño
    void TriggerFlash();
    void RenderFlash();

    // Barra vertical de progreso de profundidad
    void RenderDepthBar(float currentDepth, float maxDepth);

    // Dibuja la pantalla de menú principal
    void RenderMenu();

    // Dibuja la pantalla de Victoria
    void RenderWon();

    // Dibuja la pantalla de Game Over
    void RenderLost();

    // Agrega un texto con daño o recuperación que sube flotando desde una coordenada
    void AddPopup(float x, float y, const char* text, float r, float g, float b);

    // Sistema de guardado High Score temporal en memoria
    void SetHighScore(float score);
    float GetHighScore() const;

private:
    std::vector<FloatingText> m_popups;
    float m_highScore;

    // Flash de colisión
    float m_flashTimer;     // Tiempo restante del flash
};

#endif
