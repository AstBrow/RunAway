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
    float speed;
 
 public:
    void virtual Update() = 0;

    float  getX() { return x; }
    float  getY() { return y; }
    float  getSpeed() { return speed; }

    void setX(float newX) { x = newX; };
    void setY(float newY) { y = newY; };
    void setSpeed(float newSpeed) { speed = newSpeed; };

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
        if (newHealth >= 0) health = newHealth;
        else health = 1; 
    };

};

class Bullet : public Object 
{
  private:

    float width, height;
    float angle;
    int damage;
    bool isActive;

  public: 

    Bullet () 
    {
        setSpeed(8);
        setWidth(8);
        setHeight(10);
        setDamage(1);
        setActive(false);
    }

    Bullet (float _speed, float _width, float _height, int _damage) 
    {
        setSpeed(_speed);
        setWidth(_width);
        setHeight(_height);
        setDamage(_damage);
        setActive(false);
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
    bool isAlive;

    float invulnTimer = 0.0f;
    float invuln_duration = 1.0f; //  сек неуязвимости

 public:

    Player () 
    {
        setHealth(8);
        setX(GetScreenWidth()/2);
        setY(GetScreenHeight()/2);
        SetShootCooldown(0.3f);
        SetIsAlive(true);
        bullets.resize(1000);

        setSpeed(5);
    }

    Player (int _health, float _speed, float _shootCooldown, int sizeBullets, float _invulm_duration) 
    {
        setHealth(_health);
        setX(GetScreenWidth()/2);
        setY(GetScreenHeight()/2);
        SetShootCooldown(_shootCooldown);
        SetIsAlive(true);
        SetInvulmDuration(_invulm_duration);
        bullets.resize(sizeBullets);

        setSpeed(_speed);
    }

    void Update () override {}

    void Update(float angleDeg)  
    {
        if (isAlive)
        {
            if (invulnTimer > 0.0f) { invulnTimer -= GetFrameTime(); }

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
    }

    void Draw () override
    {
        if (GetInvulmTimer() > 0.0f && (int)(GetTime() * 10) % 2 == 0) { return; }

        if (isAlive) 
        {
            DrawCircle(getX(), getY(), radius, BLUE);

            for (auto &bul : bullets) 
            {
                if (bul.getIsActive()) 
                {
                    bul.Draw();
                }
            }
        }
    }

    void TriggerInvulnerability () { invulnTimer = invuln_duration; }

    float GetShootCooldown () { return shootCooldown; }
    std::vector<Bullet>& GetBullets () { return bullets; }
    Vector2 GetVector2 () { return Vector2{getX(), getY()}; }
    float GetRadius () { return radius; }
    bool GetisAlive () { return isAlive; }
    float GetInvulmTimer () { return invulnTimer; }
    float GetInvulmDuration () { return invuln_duration; };

    void SetShootCooldown (float _shootCooldown) 
    {
        if (_shootCooldown >= 0.0f) 
        {
            shootCooldown = _shootCooldown;
        }
        else shootCooldown = 0.0f;
    }
    void SetRadius (float _radius) 
    {
        if (_radius > 0) radius = _radius;
        else radius = 1;
    }
    void SetIsAlive (bool _isAlive) { isAlive = _isAlive; }
    void SetInvulmTimer (float _invulmTimer) 
    {
        if (_invulmTimer >= 0.0f) invulnTimer = _invulmTimer;
        else invulnTimer = 0.0f;
    }
    void SetInvulmDuration (float _invulmDuration) 
    {
        if (_invulmDuration >= 0.0f) invuln_duration = _invulmDuration;
        else invuln_duration = 0.5f;
    }
};

class Enemy : public Entity 
{
  private:

    Vector2 position;
    float angelDeg;
    int damage;
    float width, height;
    bool isAlive = false;
    Color color;

  public:
    
    Enemy() 
    {
        setHealth(1);
        setX(randX());
        setY(randY());
        setWidth(15);
        setHeight(15);
        setSpeed(3);
        setAlive(true);
        setDamage(1);
        setColor(RED);
    }

    Enemy (float _x, float _y) : Enemy(1, _x, _y, 15, 15, 3, 1) {}
    
    Enemy (int _health, float _x, float _y, float _width, float _height, float _speed, int _damage) 
    {
        setHealth(_health);
        setX(_x);
        setY(_y);
        setWidth(_width);
        setHeight(_height);
        setSpeed(_speed);
        setDamage(_damage);
        setAlive(true);
        setColor(RED);
    }

    void Update () override {}

    void Update(float playerPosX, float playerPosY)  
    {
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
            DrawRectangle(getX(), getY(), getWidth(), getHeight(), color);
        }
    }

    float getWidth () { return width; }
    float getHeight () { return height; }
    bool getIsAlive () { return isAlive; }
    int getDamage () { return damage; }
    Color getColor () { return color; }

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
    void setDamage (int _damage) 
    {
        if (_damage >= 0) damage = _damage;
        else damage = 0;
    }
    void setColor (Color _color) { color = _color; }

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
        DrawRectangle(position.x - size / 2, position.y - thickness / 2, size, thickness, DARKBLUE);
        DrawRectangle(position.x - thickness / 2, position.y - size / 2, thickness, size, DARKBLUE);
    }

    Vector2 getPosition () { return position; }

    float getAngleDeg () { return angleDeg; }

};

class GameProcess 
{
  private:

    int diffLvl;
    float spawnTimer;
    float spawnInterval = 3.0f;
    int qualityEnemy;
    int score;
    int oldScore;
    bool isUpgradeStage1 = false;
    bool isUpgradeStage2 = false;
    std::vector<Enemy> enemies;

    float defaultSpeedEnemy = 3.0f;
    int defaultEnemyHealth = 1;
    float defaultWidth = 15;
    float defaultHeight = 15;
    Color defaultEnemyColor = RED; 

    bool gameOver = false;
    bool pauseGame = false;

  public:

    GameProcess (int diffLvl, float spawnTimer, float spawnInterval, int qualityEnemy, int sizeVecEnemy) 
    : diffLvl(diffLvl), spawnTimer(spawnTimer), spawnInterval(spawnInterval), qualityEnemy(qualityEnemy) 
    {
        enemies.reserve(sizeVecEnemy);
        oldScore = 0;
        score = 0;
    }

    GameProcess () : diffLvl(1), spawnTimer(0), spawnInterval(3.0f), qualityEnemy(5) 
    {
        enemies.reserve(500);
        oldScore= 0;
        score = 0;
    }

    void Update (std::unique_ptr<Player>& player) 
    {
        if (player->getHealth() <= 0) gameOver = true;

        if (!gameOver)
        {
            spawnTimer += GetFrameTime();
            if (spawnTimer >= spawnInterval) 
            {
                for (int i = 0; i < qualityEnemy; i++) 
                {
                    Enemy enem;
                    enem.setHealth(defaultEnemyHealth);
                    enem.setColor(defaultEnemyColor);
                    enem.setSpeed(defaultSpeedEnemy);
                    enem.setHeight(defaultHeight);
                    enem.setWidth(defaultWidth);
                    enemies.push_back(enem);
                }
                spawnTimer = 0.0f;
            }

            if (score >= oldScore + 50) 
            {
                oldScore = score;

                spawnInterval = spawnInterval * 0.9f;
                if (spawnInterval < 0.3f) spawnInterval = 0.3f;

                qualityEnemy = (int)(qualityEnemy * 1.15f); 

                defaultSpeedEnemy = defaultSpeedEnemy * 1.10f;
                if (defaultSpeedEnemy > player->getSpeed()) defaultSpeedEnemy = player->getSpeed();

                if (score >= 150) diffLvl = 2;

                if (score >= 250) diffLvl = 3;
            }

            if (diffLvl == 2 && !isUpgradeStage1) 
            {
                isUpgradeStage1 = true;

                defaultEnemyColor = DARKGREEN;
                defaultEnemyHealth = 2;

                player->SetShootCooldown(0.15f);
            }

            if (diffLvl == 3 && !isUpgradeStage2) 
            {
                isUpgradeStage2 = true;

                defaultEnemyColor = VIOLET;
                defaultEnemyHealth = 2;
                defaultHeight = 20;
                defaultWidth = 20;

                player->setSpeed(5.5f);
                player->SetShootCooldown(0.11f);
            }
        }
        
    }

    void Draw (int playerHealth) 
    {
        ClearBackground(LIGHTGRAY);
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 24, BLACK);
        DrawText(TextFormat("Health: %i", playerHealth), 10, 40, 32, BLACK);
        DrawText(TextFormat("Score: %i", score), 10, 70, 32, BLACK);

        if (pauseGame) 
        {
            DrawText("PAUSE", GetScreenWidth()/4 + 220, GetScreenHeight()/2 - 90, 48, BLACK);
        }

        if (gameOver) 
        {
            DrawText("GAME OVER", GetScreenWidth()/4 + 150, GetScreenHeight()/2 - 90, 48, BLACK);
            DrawText("Press ENTER to restart..", GetScreenWidth()/4 + 90, GetScreenHeight()/2, 32, BLACK);
            DrawText(TextFormat("Score: %i", score), GetScreenWidth()/4 + 220, GetScreenHeight()/2 + 45, 32, BLACK);
        }

    }

    void Reset (std::unique_ptr<Player>& player) 
    {
        defaultEnemyColor = RED;
        defaultEnemyHealth = 1;
        defaultSpeedEnemy = 3.0f;
        isUpgradeStage1 = false;
        isUpgradeStage2 = false;

        score = 0;
        oldScore = 0;
        qualityEnemy = 5;
        spawnInterval = 3.0f;
        diffLvl = 1;

        enemies.clear();
        player->GetBullets().clear();
        player->GetBullets().resize(1000);
        player->setX(GetScreenWidth()/2);
        player->setY(GetScreenHeight()/2);
        player->setHealth(3);
        player->SetShootCooldown(0.3f);
        player->SetIsAlive(true);
        player->SetInvulmDuration(3.0f);
        player->SetInvulmTimer(0.0f);
        player->setSpeed(5);

        gameOver = false;
    }

    void Pause () 
    {
        if (pauseGame) pauseGame = false;
        else pauseGame = true;
    }

    std::vector<Enemy>& GetEnemies () { return enemies; }
    int GetScore () { return score; }
    bool GetGameOver () { return gameOver; }
    bool GetPauseGame () { return pauseGame; }
    
    void SetScore (int _score) 
    {
        if (_score >= 0) score = _score;
        else score = 0; 
    }
    void SetGameOver (bool _gameOver) { gameOver = _gameOver; }
    void setPauseGame (bool _pause) { pauseGame = _pause; }
};

class Bonus : public Object
{
  private:
    bool isActive;
    bool wasTake;
    float radius;
    
  public:

    virtual ~Bonus() = default;

    void  Update () {}
    void virtual Draw () = 0;

    bool getActive () { return isActive; }
    bool getWasTake () { return wasTake; }
    Vector2 GetVector2 () { return Vector2{getX(), getY()}; }
    float getRadius () { return radius; }

    void setWasTake (bool _wasTake) { wasTake = _wasTake; }
    void setRadius (float _radius) 
    {
        if (_radius > 0.0f) 
        {
            radius = _radius;
        }
        else radius = 1.0f;
    }
    void setActive (bool _isActive) { isActive = _isActive; }
};

class SpeedBonus : public Bonus 
{
  private:
    float bonusSpeed = 5.0f;
    float durationBonus = 8.0f;
    float durationLife = 5.0f;
    int oldScore;

    float oldPlayerSpeed;
    float currentPlayerSpeed;

    bool lockUpdateSpeed = false;
    bool lockRandomCoord = false;

  public:
    
    SpeedBonus () 
    { 
        setRadius(8.0f); 
        setActive(false);
    }

    void Update (int score, std::unique_ptr<Player>& player) 
    {
        if (score >= oldScore + 50) 
        {
            oldScore = score;

            int chance = 1 + rand() % 100;
            if (chance >= 1 && chance <= 100) 
            {
                setActive(true);
            }
        }

        if (getActive()) 
        {
            if (!getWasTake())
            {
                durationLife -= GetFrameTime();
                if (durationLife <= 0.0f) 
                {
                    setActive(false);
                    durationLife = 5.0f;
                }

                if (!lockRandomCoord) 
                {
                    lockRandomCoord = true;

                    float newX, newY;
                    newX = 0 + rand() % 1200;
                    newY = 0 + rand() % 800;

                    setX(newX);
                    setY(newY);
                }
            }
            else 
            {

                if (!lockUpdateSpeed) 
                {
                    oldPlayerSpeed = player->getSpeed();
                    currentPlayerSpeed = player->getSpeed() + bonusSpeed;
                    lockUpdateSpeed = true;
                }

                durationBonus -= GetFrameTime();
                if (durationBonus <= 0.0f) 
                {
                    setActive(false);
                    setWasTake(false);
                    player->setSpeed(oldPlayerSpeed);
                    lockUpdateSpeed = false;
                    lockRandomCoord = false;
                    durationBonus = 8.0f;
                }
                else 
                {
                    player->setSpeed(currentPlayerSpeed);
                }
            }
        }
    }

    void Draw () 
    {
        if (getActive()) 
        {
            if (!getWasTake())
            {
                if (durationLife <= 2.0f && (int)(GetTime() * 10) % 2 == 0) return; 
                DrawCircle(getX(), getY(), getRadius(), YELLOW);
            }

            if (getWasTake()) 
            {
                DrawRectangle(1100, 750, 100, 50, BLACK);
            }
        }
    }

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

    void CheckcollisionBulletAndEnemy (Bullet &bul, Enemy &enem, std::unique_ptr<GameProcess> &process) 
    {
        if (CheckCollisionRecs(bul.getRect(), enem.getRect())) 
        {
            int newHealth = enem.getHealth() - bul.getDamage();

            if (newHealth <= 0) 
            {
                newHealth = 0;
                enem.setAlive(false);
                
                int newScore = process->GetScore() + 1;
                process->SetScore(newScore);
            }
            enem.setHealth(newHealth);
            bul.setActive(false);
        }
    }
    void CheckCollisionAllBullet (std::vector<Bullet> &bullets, std::vector<Enemy> &enemies, std::unique_ptr<GameProcess> &process) 
    {
        for (int i = 0; i < bullets.size(); i++) 
        {
            if (!bullets[i].getIsActive()) continue;

            for (int j = 0; j < enemies.size() ; j++) 
            {
                if (enemies[j].getIsAlive()) 
                {
                    CheckcollisionBulletAndEnemy(bullets[i], enemies[j], process);

                    if (!bullets[i].getIsActive()) break;
                }
            }
        }
    }

    void CheckCollisionPlayerAndEnemy (std::unique_ptr<Player> &player, Enemy &enem) 
    {
        if (player->GetInvulmTimer() > 0.0f) return;

        if (CheckCollisionCircleRec(player->GetVector2(), player->GetRadius(), enem.getRect())) 
        {
            int newHealth = player->getHealth() - enem.getDamage();

            if (newHealth <= 0) 
            {
                newHealth = 0;
                player->SetIsAlive(false);
            }
            player->setHealth(newHealth);

            player->TriggerInvulnerability();
        }
    }
    void CheckCollisionPlayerAndAllEnemy (std::unique_ptr<Player> &player, std::vector<Enemy> &enemies) 
    {
        for (int i = 0; i < enemies.size(); i++) 
        {
            if (!enemies[i].getIsAlive()) continue;

            CheckCollisionPlayerAndEnemy(player, enemies[i]);
        }
    }

    void CheckCollisionPlayerAndBonus (std::unique_ptr<Player> &player, const std::shared_ptr<SpeedBonus> &bonus)
    {
        if (CheckCollisionCircles(player->GetVector2(), player->GetRadius(), bonus->GetVector2(), bonus->getRadius())) 
        {
            bonus->setWasTake(true);
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
    std::unique_ptr<CollisionManager> collission = std::make_unique<CollisionManager>();

    std::unique_ptr<GameProcess> process = std::make_unique<GameProcess>();
    std::unique_ptr<Player> player = std::make_unique<Player>();
    std::shared_ptr<SpeedBonus> speedBonus = std::make_shared<SpeedBonus>();
    std::unique_ptr<Cursor> cursor = std::make_unique<Cursor>();


    while(WindowShouldClose() == false) 
    {
        // Updating            

        cursor->Update(player->getX(), player->getY());

        if (IsKeyPressed(KEY_P)) 
        {
            process->Pause();
        }

        if (!process->GetPauseGame())
        {
            if (!process->GetGameOver()) 
            {
                for(int i = 0; i < process->GetEnemies().size(); i++) 
                {
                    process->GetEnemies()[i].Update(player->getX(), player->getY());
                }

                player->Update(cursor->getAngleDeg());
                process->Update(player);
                speedBonus->Update(process->GetScore(), player);

                // Checking collision

                collission->CheckCollissionAllEnemy(process->GetEnemies());
                collission->CheckCollisionAllBullet(player->GetBullets(), process->GetEnemies(), process);
                collission->CheckCollisionPlayerAndAllEnemy(player, process->GetEnemies());
                collission->CheckCollisionPlayerAndBonus(player, speedBonus);
            }
            else 
            {
                if (IsKeyPressed(KEY_ENTER)) process->Reset(player);
            }
        }

        // Drawing

        BeginDrawing();

        player->Draw();
        for(int i = 0; i < process->GetEnemies().size(); i++) 
        {
            process->GetEnemies()[i].Draw();
        }
        cursor->Draw();
        process->Draw(player->getHealth());
        speedBonus->Draw();
        
        EndDrawing();
    }

}