#include "GameObject.h"
#include "../ContestAPI/app.h"

GameObject::GameObject(const char* spriteName, int cols, int rows)
{
    // Usamos la API de App para crear el sprite
    m_sprite = App::CreateSprite(spriteName, cols, rows);
    m_x = 0.0f;
    m_y = 0.0f;
    m_scale = 1.0f; // Escala por defecto
}

GameObject::~GameObject()
{
    if (m_sprite)
    {
        delete m_sprite;
        m_sprite = nullptr;
    }
}

void GameObject::Update(float dt)
{
    if (m_sprite)
    {
        m_sprite->SetPosition(m_x, m_y);
        m_sprite->SetScale(m_scale); // Aplicar siempre la escala guardada
        m_sprite->Update(dt);
    }
}

void GameObject::Draw()
{
    if (m_sprite)
    {
        m_sprite->Draw();
    }
}

void GameObject::SetPosition(float x, float y)
{
    m_x = x;
    m_y = y;
    if (m_sprite)
    {
        m_sprite->SetPosition(m_x, m_y);
    }
}

void GameObject::GetPosition(float &x, float &y)
{
    x = m_x;
    y = m_y;
}

void GameObject::SetScale(float scale)
{
    m_scale = scale; // Guardar la escala
    if (m_sprite)
    {
        m_sprite->SetScale(m_scale);
    }
}

float GameObject::GetWidth()
{
    return (m_sprite) ? m_sprite->GetWidth() : 0.0f;
}

float GameObject::GetHeight()
{
    return (m_sprite) ? m_sprite->GetHeight() : 0.0f;
}