#include <iostream>
#include <memory>
#include <vector>
#include <math.h>
#include "raylib.h"

class Object 
{
 private:
    float x, y;
    int speed;
 
 public:
    void virtual Update() = 0;

    float  getX() { return x; }
    float  getY() { return y; }
    int  getSpeed() { return speed; }

    void setX(float newX) { x = newX; };
    void setY(float newY) { y = newY; };
    void setSpeed(int newSpeed) { speed = newSpeed; };

};

class Entity : public Object
{
 private:
    int health;

 public:
    void virtual Update() = 0;
    void virtual Draw() = 0;

    int  getHealth() { return health; }
    void setHealth(int newHealth) { health = newHealth; };

};


class Bullet : public Object 
{
  private:

    float angle;
    bool isActive = false;

  public: 

    Bullet () 
    {
        setSpeed(8);
    }

    Bullet (int _speed) 
    {
        setSpeed(_speed);
    }

    void Update () override {}

    void Update (float playerPosX, float playerPosY, float playerShootCooldown, float angleDeg)    
    {
        float newX, newY;
        
        float currentAngle = getAngle();

        newX = (getSpeed() * cos(currentAngle * DEG2RAD)) + getX();
        newY = (getSpeed() * sin(currentAngle * DEG2RAD)) + getY();

        if ((newX > GetScreenWidth() || newX < 0) || (newY > GetScreenHeight() || newY < 0)) 
        {
            isActive = false;
        }
        else    
        {
            setX(newX);
            setY(newY);
        }
    }

    void Draw () 
    {
        DrawRectangle(getX(), getY(), 8, 10, BLACK);
    }

    float getAngle () { return angle; }
    
    bool getIsActive () { return isActive; }

    void setAngle (float _angle) 
    {
        angle = _angle;
    }

    void setActive (bool _isActive) 
    {
        isActive = _isActive;
    }

};

class Player : public Entity 
{
 private:
    std::vector<Bullet> bullets;

    float radius = 12;
    float shootCooldown; // Задержка между выстрелами (в секундах)
    float lastShootTime;

 public:

    Player () 
    {
        setHealth(3);
        setX(GetScreenWidth()/2);
        setY(GetScreenHeight()/2);
        SetShootCooldown(0.3f);
        bullets.resize(1000);

        setSpeed(4);
    }

    Player (int _health, int _speed, float _shootCooldown, int sizeBullets) 
    {
        setHealth(_health);
        setX(GetScreenWidth()/2);
        setY(GetScreenHeight()/2);
        SetShootCooldown(_shootCooldown);
        bullets.resize(sizeBullets);

        setSpeed(_speed);
    }

    void Update () override {}

    void Update(float angleDeg)  
    {
        int speedTemp;
        float xTemp, yTemp;

        speedTemp = getSpeed();
        xTemp = getX();
        yTemp = getY();

        if (IsKeyDown(KEY_W)) yTemp -= speedTemp;
        
        if (IsKeyDown(KEY_S)) yTemp += speedTemp;
        
        if (IsKeyDown(KEY_A)) xTemp -= speedTemp;

        if (IsKeyDown(KEY_D)) xTemp += speedTemp;

        if (yTemp < radius) yTemp = radius;
        if (yTemp > GetScreenHeight() - radius) yTemp = GetScreenHeight() - radius;

        if (xTemp < radius) xTemp = radius;
        if (xTemp > GetScreenWidth() - radius) xTemp = GetScreenWidth() - radius;

        setY(yTemp);
        setX(xTemp);

        float currentTime = GetTime();

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            if (currentTime - lastShootTime >= shootCooldown) 
            {
                for (auto &bul : bullets) 
                {
                    if (!bul.getIsActive()) 
                    {
                        bul.setActive(true);
                        bul.setAngle(angleDeg);
                        bul.setX(getX());
                        bul.setY(getY());
                        
                        break;
                    }
                }

                lastShootTime = currentTime;
            }

        }

        for (auto &bul : bullets) 
        {
            if (bul.getIsActive())
            {
                bul.Update(getX(), getY(), shootCooldown, angleDeg);
            }
        }
    }

    void Draw () override
    {
        DrawText(TextFormat("Health: %i", getHealth()), 10, 40, 24, BLACK);
        DrawCircle(getX(), getY(), radius, BLUE);

        for (auto &bul : bullets) 
        {
            if (bul.getIsActive()) 
            {
                bul.Draw();
            }
        }
    }

    float GetShootCooldown () { return shootCooldown; }

    void SetShootCooldown (float _shootCooldown) 
    {
        if (_shootCooldown >= 0.0f) 
        {
            shootCooldown = _shootCooldown;
        }
    }

};

class Enemy : public Entity 
{
  private:
    Vector2 position;
    float angelDeg;

  public:
    
    Enemy(int startX, int startY) 
    {
        setHealth(1);

        setX(startX);
        setY(startY);

        setSpeed(3);
    }

    void Update () override {}

    void Update(float playerPosX, float playerPosY)  
    {
        position.x = playerPosX;
        position.y = playerPosY;
        angelDeg = atan2f(position.y - getY(), position.x - getX()) * RAD2DEG;

        float newX, newY;
        newX = (getSpeed() * cos(angelDeg * DEG2RAD)) + getX();
        newY = (getSpeed() * sin(angelDeg * DEG2RAD)) + getY();

        setX(newX);
        setY(newY);
    }

    void Draw() override
    {
        DrawRectangle(getX(), getY(), 15,15, RED);
    }

};

class Cursor 
{
 private:
    Vector2 position;
    float radius;

    int size = 14; // размер 
    int thickness = 2; // толщина

    float angleDeg; // угол atan2

 public:
    
    Cursor() : position({0,0}) 
    {
        HideCursor();
    }

    void Update (int playerPosX, int playerPosY) 
    {
        position = GetMousePosition();
        angleDeg = atan2f(position.y - playerPosY ,position.x - playerPosX) * RAD2DEG; 
    }

    void Draw () 
    {
        DrawRectangle(position.x - size / 2, position.y - thickness / 2, size, thickness, BLACK);
        DrawRectangle(position.x - thickness / 2, position.y - size / 2, thickness, size, BLACK);
    }

    Vector2 getPosition () { return position; }

    float getAngleDeg () { return angleDeg; }

};



int main () 
{
    int widthWi = 1200;  //x
    int heigthWi = 800;  //y

    InitWindow(widthWi, heigthWi, "RunAway");
    SetTargetFPS(60);

    //Initialization

    std::unique_ptr<Player> player = std::make_unique<Player>();
    std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(360, 200);
    std::unique_ptr<Enemy> enemy1 = std::make_unique<Enemy>(300, 140);

    std::unique_ptr<Cursor> cursor = std::make_unique<Cursor>();

    while(WindowShouldClose() == false) 
    {
        // Updating
        
        cursor->Update(player->getX(), player->getY());
        player->Update(cursor->getAngleDeg());
        enemy->Update(player->getX(), player->getY());
        enemy1->Update(player->getX(), player->getY());

        //Drawing

        BeginDrawing();

        ClearBackground(LIGHTGRAY);
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 24, BLACK);
        player->Draw();
        enemy->Draw();
        enemy1->Draw();
        cursor->Draw();
        

        EndDrawing();
    }
}