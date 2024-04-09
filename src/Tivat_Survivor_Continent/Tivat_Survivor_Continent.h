#ifndef _TIVAT_SURIVIVOR_CONTINET_H_
#define _TIVAT_SURIVIVOR_CONTINET_H_

#include <stdio.h>
#include <iostream>
#include <easyx.h>
#include <unistd.h>
#include <math.h>
#include <string>
#include <vector>
#include <mmsystem.h>

#endif // !TEST_H

void putimage_alpha(int x, int y, IMAGE *image);

class Atlas
{
private:
public:
    std::vector<IMAGE *> frame_list;

public:
    Atlas(LPCTSTR path, int num);

    ~Atlas();
};

class Animation
{
public:
    Animation(Atlas *atlas, int interval);

    ~Animation();

public:
    void Play(int x, int y, int delta);

private:
    int timer = 0;
    int idx_frame = 0;
    int interval_ms = 0;

private:
    Atlas *anim_atles;
};

class Bullet
{
private:
    const int RADIUS = 10;
    const double DAMAGE = 20;

public:
    POINT postion = {0, 0};

    void Draw() const;

    double GetDAMAGE() const
    {
        return DAMAGE;
    }

public:
    Bullet();
    ~Bullet();
};

class Player
{
private:
    const double PLAYER_SPEED = 4;
    const double PLAYER_HEALTH = 100;

    const int PLAYER_WIDTH = 80;
    const int PLAYER_HEIGHT = 80;

    const int SHADOW_WIDTH = 32;

    const int HEALTH_WIDTH = 32;
    /* data */
private:
    bool is_player_up = false;
    bool is_player_down = false;
    bool is_player_left = false;
    bool is_player_right = false;

    POINT player_position = {500, 500};
    IMAGE img_shadow;
    IMAGE img_health;
    IMAGE img_background;

    Animation *anim_player_left;
    Animation *anim_player_right;

public:
    Player();
    ~Player();

public:
    void ProcessEvent(const ExMessage &msg);

    void Move();

    void Draw(int delta);

    const int &GetPlayerWidth() const
    {
        return PLAYER_WIDTH;
    }
    const int &GetPlayerHeight() const
    {
        return PLAYER_HEIGHT;
    }
    const POINT &GetPosition() const
    {
        return player_position;
    }
};
class Enemy
{
private:
    const double ENEMY_SPEED = 3;

    const int ENEMY_WIDTH = 96;
    const int ENEMY_HEIGHT = 96;

    const int SHADOW_WIDTH = 48;
    const int HEALTH_WIDTH = 32;

    const int ENEMY_DAMAGE = 20;

private:
    Animation *anim_enemy_left;
    Animation *anim_enemy_right;

    double ENEMY_HEALTH = 800;
    // double anim_enemy_health = 100;

    POINT position = {0, 0};

    IMAGE img_shadow;
    IMAGE img_health;

    bool facing_left = false;
    bool alive = true;

    Player player;

public:
    bool CheckBulletColetCollision(const Bullet &bullet);

    bool CheckPlayerColetCollision(const Player &player);

    void Move(const Player &player);

    void Draw(int delta);

    void Hurt(double DAMAGE)
    {
        ENEMY_HEALTH -= DAMAGE;
        loadimage(&img_health, _T("../Teyvat Survivors/Teyvat Survivor (Advanced)/img/enemy_health_5.png"));
        putimage_alpha(position.x + (ENEMY_WIDTH / 2 - HEALTH_WIDTH / 2), position.y + 5, &img_health);

        if (ENEMY_HEALTH <= 0)
            alive = false;
    }
    bool CheckAlive()
    {
        return alive;
    }

public:
    Enemy();
    ~Enemy();
};

class Button
{
protected:
    virtual void OnClick() = 0;

private:
    enum class Status
    {
        Idle = 0,
        Hovered,
        Pushed,
    };

private:
    RECT region;
    IMAGE img_idle;
    IMAGE img_hovered;
    IMAGE img_pushed;
    Status status = Status::Idle;

private:
    bool CheckCursorHit(int x, int y);

public:
    void Draw();

    void ProcessEvent(const ExMessage &msg);

public:
    Button(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed);

    ~Button() = default;
};
class StartGameButton : public Button
{
protected:
    void OnClick();

private:
public:
    StartGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed);
    ~StartGameButton() = default;
};
class QuitGameButton : public Button
{
protected:
    void OnClick();

private:
public:
    QuitGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed);
    ~QuitGameButton() = default;
};

void Teyvat_Run();
