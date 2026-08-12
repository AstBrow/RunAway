#include <iostream>
#include <memory>
#include <vector>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include "raylib.h"

float randX () 
{
    float randX;
    int random_number = std::rand() % 2; 

    if (random_number == 1) randX = -(std::rand() % 99 + 1);
    else randX = std::rand() % 99 + 1201;;

    return randX;
}
float randY () 
{
    float randY;
    int random_number = std::rand() % 2; 

    if (random_number == 1) randY = -(std::rand() % 199 + 1);
    else randY = std::rand() % 199 + 801;

    return randY;
}

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

    void setHealth(int newHealth) 
    { 
        if (newHealth > 0) health = newHealth;
        else health = 1; 
    };

};

class Bullet : public Object 
{
  private:

    float width, height;
    float angle;
    int damage;
    bool isActive = false;

  public: 

    Bullet () 
    {
        setSpeed(8);
        setWidth(8);
        setHeight(10);
        setDamage(1);
    }

    Bullet (int _speed, float _width, float _height, int _damage) 
    {
        setSpeed(_speed);
        setWidth(_width);
        setHeight(_height);
        setDamage(_damage);
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
        DrawRectangle(getX(), getY(), getWidth(), getHeight(), BLACK);
    }

    float getAngle () { return angle; }
    bool getIsActive () { return isActive; }
    float getWidth () { return width; }
    float getHeight () { return height; }
    int getDamage () { return damage; }

    Rectangle getRect()  
    {
        return Rectangle{ getX(), getY(), getWidth(), getHeight() };
    }

    void setDamage (int _damage) 
    {
        if (_damage > 0) damage = _damage;
        else damage = 1;
    }
    void setAngle (float _angle) 
    {
        angle = _angle;
    }

    void setActive (bool _isActive) 
    {
        isActive = _isActive;
    }

    void setWidth (float _width) 
    {
        if (_width > 0.0f) width = _width;
        else width = 1.0f;
    }
    void setHeight (float _height) 
    {
        if (_height > 0.0f) height = _height;
        else height = 1.0f;
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
        else shootCooldown = 0.0f;
    }

};

class Enemy : public Entity 
{
  private:

    Vector2 position;
    float angelDeg;
    float width, height;
    bool isAlive = false;

  public:
    
    Enemy() 
    {
        setHealth(1);
        setX(randX());
        setY(randY());
        setWidth(15);
        setHeight(15);
        setSpeed(3);
    }

    Enemy (float _x, float _y) : Enemy(1, _x, _y, 15, 15, 3) {}
    
    Enemy (int _health, float _x, float _y, float _width, float _height, int _speed) 
    {
        setHealth(_health);
        setX(_x);
        setY(_y);
        setWidth(_width);
        setHeight(_height);
        setSpeed(_speed);
    }

    void Update () override {}

    void Update(float playerPosX, float playerPosY)  
    {
        if (getHealth() <= 0) isAlive = false;

        if (isAlive) 
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
    }

    void Draw() override
    {
        if (isAlive) 
        {
            DrawRectangle(getX(), getY(), getWidth(), getHeight(), RED);
        }
    }

    float getWidth () { return width; }
    float getHeight () { return height; }
    bool getIsAlive () { return isAlive; }

    Rectangle getRect()  
    {
        return Rectangle{ getX(), getY(), getWidth(), getHeight() };
    }

    void setAlive (bool _isAlive) { isAlive = _isAlive; }
    void setWidth (float _width) 
    {
        if (_width > 0.0f) width = _width;
        else width = 1.0f;
    }
    void setHeight (float _height) 
    {
        if (_height > 0.0f) height = _height;
        else height = 1.0f;
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

class CollisionManager 
{
  public:

    void CheckCollisisonTwoEnemy (Enemy &enem1, Enemy &enem2) 
    {
        Rectangle overlap = GetCollisionRec(enem1.getRect(), enem2.getRect());
        float newXEnem1, newYEnem1;
        newXEnem1 = enem1.getX();
        newYEnem1 = enem1.getY();

        float newXEnem2, newYEnem2;
        newXEnem2 = enem2.getX();
        newYEnem2 = enem2.getY();

        if (overlap.width > 0 && overlap.height > 0) 
        {
            if (overlap.width < overlap.height) 
            {
                if (enem1.getRect().x < enem2.getRect().x) 
                {
                    newXEnem1 -= overlap.width * 0.5f;
                    newXEnem2 += overlap.width * 0.5f;
                }
                else 
                {
                    newXEnem1 += overlap.width * 0.5f;
                    newXEnem2 -= overlap.width * 0.5f;
                }
                
                enem1.setX(newXEnem1);
                enem2.setX(newXEnem2);
            }
            else 
            {
                if (enem1.getRect().y < enem2.getRect().y) 
                {
                    newYEnem1 -= overlap.height * 0.5f;
                    newYEnem2 += overlap.height * 0.5f;
                }
                else 
                {
                    newYEnem1 += overlap.height * 0.5f;
                    newYEnem2 -= overlap.height * 0.5f;
                }

                enem1.setY(newYEnem1);
                enem2.setY(newYEnem2);
            }
        }
    }
    void CheckCollissionAllEnemy (std::vector<Enemy> &enemies) 
    {
        for (int i = 0; i < enemies.size(); i++) 
        {
            for (int j = i + 1; j < enemies.size(); j++) 
            {
                CheckCollisisonTwoEnemy(enemies[i], enemies[j]);
            }
        }
    }

    void CheckcollisionBulletAndEnemy (Bullet &bul, Enemy &enem) 
    {
        if (CheckCollisionRecs(bul.getRect(), enem.getRect())) 
        {
            bul.setActive(false);
            int newHealth = enem.getHealth() - bul.getDamage();
            enem.setHealth(newHealth);
        }
    }
    void CheckCollisionAllBullet (std::vector<Bullet> &bullets, std::vector<Enemy> &enemies) 
    {
        for (int i = 0; i < bullets.size(); i++) 
        {
            if (!bullets[i].getIsActive()) continue;

            for (int j = 0; j < enemies.size() ; j++) 
            {
                if (enemies[j].getIsAlive()) 
                {
                    CheckcollisionBulletAndEnemy(bullets[i], enemies[j]);

                    if (!bullets[i].getIsActive()) break;
                }
            }
        }
    }
};

int main () 
{
    std::srand(std::time(nullptr));

    int widthWi = 1200;  // x
    int heigthWi = 800;  // y

    InitWindow(widthWi, heigthWi, "RunAway");
    SetTargetFPS(60);

    //Initialization
 
    std::vector<Enemy> enemies;
    enemies.resize(100);

    for (int i = 0; i < 100; i++) 
    {
        Enemy enem;
        enem.setAlive(true);
        enemies.push_back(enem);
    }

    std::unique_ptr<Player> player = std::make_unique<Player>();
    std::unique_ptr<Cursor> cursor = std::make_unique<Cursor>();
    std::unique_ptr<CollisionManager> collission = std::make_unique<CollisionManager>();

    while(WindowShouldClose() == false) 
    {
        // Updating
        
        cursor->Update(player->getX(), player->getY());
        
        for(int i = 0; i < enemies.size(); i++) 
        {
            enemies[i].Update(player->getX(), player->getY());
        }

        player->Update(cursor->getAngleDeg());

        // Checking collision

        collission->CheckCollissionAllEnemy(enemies);

        // Drawing

        BeginDrawing();

        ClearBackground(LIGHTGRAY);
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 24, BLACK);
        player->Draw();

        for(int i = 0; i < enemies.size(); i++) 
        {
            enemies[i].Draw();
        }
        cursor->Draw();
        
        EndDrawing();
    }
}