#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include "../ContestAPI/SimpleSprite.h"

class GameObject
{
public:
    GameObject(const char* spriteName, int cols = 1, int rows = 1);
    virtual ~GameObject();

    virtual void Update(float dt);
    virtual void Draw();

    void SetPosition(float x, float y);
    void GetPosition(float &x, float &y);
    void SetScale(float scale);
    float GetWidth();
    float GetHeight();

 
    CSimpleSprite* GetSprite() { return m_sprite; }

protected:
    CSimpleSprite* m_sprite;
    float m_x;
    float m_y;
    float m_scale; 
};

#endif
