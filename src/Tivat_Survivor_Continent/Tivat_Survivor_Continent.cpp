#include "Tivat_Survivor_Continent.h"

Atlas *atlas_player_left;
Atlas *atlas_player_right;
Atlas *atlas_enemy_left;
Atlas *atlas_enemy_right;

Atlas *atlas_enemy_health;

bool running = true;
bool is_game_started = false;

const int PLAYER_ANIM_NUM = 6;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const int BUTTON_WIDTH = 192;
const int BUTTON_HEIGHT = 75;

int idx_current_anim = 0;

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];

inline void putimage_alpha(int x, int y, IMAGE *image)
{
    int w = image->getwidth();
    int h = image->getheight();
    AlphaBlend(GetImageHDC(NULL), x, y, w, h,
               GetImageHDC(image), 0, 0, w, h, {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA});
}

Atlas::Atlas(LPCTSTR path, int num)
{
    TCHAR path_file[256];
    for (int i = 0; i < num; i++)
    {
        _stprintf_s(path_file, path, i);
        IMAGE *frame = new IMAGE();
        loadimage(frame, path_file);
        frame_list.push_back(frame);
    }
}

Atlas::~Atlas()
{
    for (long long unsigned i = 0; i < frame_list.size(); i++)
    {
        delete frame_list[i];
    }
}
Animation::Animation(Atlas *atlas, int interval)
{
    anim_atles = atlas;
    interval_ms = interval;
}
Animation::~Animation()
{
}

void Animation::Play(int x, int y, int delta)
{
    timer += delta;
    if (timer >= interval_ms)
    {
        idx_frame = (idx_frame + 1) % anim_atles->frame_list.size();
        timer = 0;
    }
    putimage_alpha(x, y, anim_atles->frame_list[idx_frame]);
}

Bullet::Bullet()
{
}

Bullet::~Bullet()
{
}
void Bullet::Draw() const
{
    setlinecolor(RGB(255, 125, 50));
    setfillcolor(RGB(200, 75, 10));
    fillcircle(postion.x, postion.y, RADIUS);
}

Player::Player()
{
    loadimage(&img_shadow, (LPCTSTR)("../package/Teyvat Survivor (Advanced)/img/shadow_player.png"), 0, 0, false);
    loadimage(&img_health, (LPCTSTR)("../package/Teyvat Survivor (Advanced)/img/enemy_health_5.png"), 0, 0, false);
    atlas_player_left = new Atlas(_T("../package/Teyvat Survivor (Advanced)/img/paimon_left_%d.png"), 6);
    atlas_player_right = new Atlas(_T("../package/Teyvat Survivor (Advanced)/img/paimon_right_%d.png"), 6);
    anim_player_left = new Animation(atlas_player_left, 45);
    anim_player_right = new Animation(atlas_player_right, 45);
}

Player::~Player()
{
    delete anim_player_left;
    delete anim_player_right;
}

void Player::ProcessEvent(const ExMessage &msg)
{
    switch (msg.message)
    {
    case WM_KEYDOWN:
        switch (msg.vkcode)
        {
        case VK_UP:
            is_player_up = true;
            break;
        case VK_DOWN:
            is_player_down = true;
            break;
        case VK_LEFT:
            is_player_left = true;
            break;
        case VK_RIGHT:
            is_player_right = true;
            break;
        }
        break;
    case WM_KEYUP:
        switch (msg.vkcode)
        {
        case VK_UP:
            is_player_up = false;
            break;
        case VK_DOWN:
            is_player_down = false;
            break;
        case VK_LEFT:
            is_player_left = false;
            break;
        case VK_RIGHT:
            is_player_right = false;
            break;
        }
    }
}
void Player::Move()
{
    int dir_x = is_player_right - is_player_left;
    int dir_y = is_player_down - is_player_up;
    double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);

    if (len_dir != 0)
    {
        double normalized_x = dir_x / len_dir;
        double normalized_y = dir_y / len_dir;
        player_position.x += (int)(PLAYER_SPEED * normalized_x);
        player_position.y += (int)(PLAYER_SPEED * normalized_y);
    }

    if (player_position.x < 0)
        player_position.x = 0;
    if (player_position.y < 0)
        player_position.y = 0;
    if (player_position.x + PLAYER_WIDTH > 1280)
        player_position.x = 1280 - PLAYER_WIDTH;
    if (player_position.y + PLAYER_HEIGHT > 720)
        player_position.y = 720 - PLAYER_HEIGHT;
}
void Player::Draw(int delta)
{
    int pos_shadow_x = player_position.x + (PLAYER_WIDTH / 2 - SHADOW_WIDTH / 2);
    int pos_shadow_y = player_position.y + PLAYER_HEIGHT - 8;

    int pos_health_x = player_position.x + (PLAYER_WIDTH / 2 - HEALTH_WIDTH / 2);
    int pos_health_y = player_position.y - 8;

    putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

    putimage_alpha(pos_health_x, pos_health_y, &img_health);

    static bool facing_left = false;
    int dir_x = is_player_right - is_player_left;
    if (dir_x < 0)
        facing_left = true;
    else if (dir_x > 0)
        facing_left = false;
    if (facing_left)
        anim_player_left->Play(player_position.x, player_position.y, delta);
    else
        anim_player_right->Play(player_position.x, player_position.y, delta);
}
Enemy::Enemy()
{
    loadimage(&img_shadow, (LPCTSTR)("../package/Teyvat Survivor (Advanced)/img/shadow_enemy.png"), 0, 0, false);
    // loadimage(&img_health, (LPCTSTR)("../Teyvat Survivors/Teyvat Survivor (Advanced)/img/enemy_health_5.png"), 0, 0, false);
    atlas_enemy_left = new Atlas(_T("../package/Teyvat Survivor (Advanced)/img/boar_left_%d.png"), 6);
    atlas_enemy_right = new Atlas(_T("../package/Teyvat Survivor (Advanced)/img/boar_right_%d.png"), 6);

    // atlas_enemy_health = new Atlas(_T("../Teyvat Survivors/Teyvat Survivor (Advanced)/img/enemy_health_%d.png"), 6);

    anim_enemy_left = new Animation(atlas_enemy_left, 45);
    anim_enemy_right = new Animation(atlas_enemy_right, 45);

    // anim_enemy_health = new Animation(atlas_enemy_health, 45);

    enum class SpawnEdge
    {
        UP = 0,
        DOWN,
        LEFT,
        RIGHT
    };
    SpawnEdge edge = (SpawnEdge)(rand() % 4);
    switch (edge)
    {
    case SpawnEdge::UP:
        position.x = rand() % 1280;
        position.y = -ENEMY_HEIGHT;
        break;
    case SpawnEdge::DOWN:
        position.x = rand() % 1280;
        position.y = 720;
        break;
    case SpawnEdge::LEFT:
        position.x = -ENEMY_WIDTH;
        position.y = rand() % 720;
        break;
    case SpawnEdge::RIGHT:
        position.x = 1280;
        position.y = rand() % 720;
        break;
    }
}

Enemy::~Enemy()
{
    delete anim_enemy_left;
    delete anim_enemy_right;
}
bool Enemy::CheckBulletColetCollision(const Bullet &bullet)
{
    bool is_overlap_x = bullet.postion.x >= position.x && bullet.postion.x <= position.x + ENEMY_WIDTH;
    bool is_overlap_y = bullet.postion.y >= position.y && bullet.postion.y <= position.y + ENEMY_HEIGHT;
    return is_overlap_x && is_overlap_y;
}

bool Enemy::CheckPlayerColetCollision(const Player &player)
{
    POINT check_position = {position.x + ENEMY_WIDTH / 2,
                            position.y + ENEMY_HEIGHT / 2};
    bool is_overlap_x = check_position.x >= player.GetPosition().x && check_position.x <= player.GetPosition().x + player.GetPlayerWidth() - 20;
    bool is_overlap_y = check_position.y >= player.GetPosition().y && check_position.y <= player.GetPosition().y + player.GetPlayerHeight() - 20;
    return is_overlap_x && is_overlap_y;
}

void Enemy::Move(const Player &player)
{
    const POINT &player_position = player.GetPosition();
    int dir_x = player_position.x - position.x;
    int dir_y = player_position.y - position.y;
    double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
    if (len_dir != 0)
    {
        double normalized_x = dir_x / len_dir;
        double normalized_y = dir_y / len_dir;
        position.x += (int)(ENEMY_SPEED * normalized_x);
        position.y += (int)(ENEMY_SPEED * normalized_y);
    }

    if (dir_x < 0)
        facing_left = true;
    else if (dir_x > 0)
        facing_left = false;
}
void Enemy::Draw(int delta)
{
    int pos_shadow_x = position.x + (ENEMY_WIDTH / 2 - SHADOW_WIDTH / 2);
    int pos_shadow_y = position.y + ENEMY_HEIGHT - 35;

    int pos_health_x = position.x + (ENEMY_WIDTH / 2 - HEALTH_WIDTH / 2);
    int pos_health_y = position.y + 5;

    putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
    putimage_alpha(pos_health_x, pos_health_y, &img_health);

    if (facing_left)
        anim_enemy_left->Play(position.x, position.y, delta);
    else
        anim_enemy_right->Play(position.x, position.y, delta);
}

bool Button::CheckCursorHit(int x, int y)
{

    return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
}
void Button::Draw()
{
    switch (status)
    {
    case Status::Idle:
        putimage(region.left, region.top, &img_idle);
        break;
    case Status::Hovered:
        putimage(region.left, region.top, &img_hovered);
        break;
    case Status::Pushed:
        putimage(region.left, region.top, &img_pushed);
        break;
    }
}
void Button::ProcessEvent(const ExMessage &msg)
{
    switch (msg.message)
    {
    case WM_MOUSEMOVE:
        if (status == Status::Idle && CheckCursorHit(msg.x, msg.y))
            status = Status::Hovered;
        else if (status == Status::Hovered && !CheckCursorHit(msg.x, msg.y))
            status = Status::Idle;
        break;
    case WM_LBUTTONDOWN:
        if (CheckCursorHit(msg.x, msg.y))
            status = Status::Pushed;
        break;
    case WM_LBUTTONUP:
        if (status == Status::Pushed)
            OnClick();
        break;

    default:
        break;
    }
}
Button::Button(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
{
    region = rect;

    loadimage(&img_idle, path_img_idle);
    loadimage(&img_hovered, path_img_hovered);
    loadimage(&img_pushed, path_img_pushed);
}

StartGameButton::StartGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed) : Button(rect, path_img_idle, path_img_hovered, path_img_pushed)
{
}

void StartGameButton::OnClick()
{
    is_game_started = true;
}
QuitGameButton::QuitGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed) : Button(rect, path_img_idle, path_img_hovered, path_img_pushed)
{
}

void QuitGameButton::OnClick()
{
    running = false;
}
/*
void LoadAnimation()
{
    for (int i = 0; i < PLAYER_ANIM_NUM; i++)
    { // paimon_left_0.png
        std::string path = "../Teyvat Survivors/Teyvat Survivor (Advanced)/img/paimon_left_" + std::to_string(i) + ".png";
        loadimage(&img_player_left[i], (LPCTSTR)path.c_str(), 0, 0, false);
    }
    for (int i = 0; i < PLAYER_ANIM_NUM; i++)
    {
        std::string path = "../Teyvat Survivors/Teyvat Survivor (Advanced)/img/paimon_right_" + std::to_string(i) + ".png";

        loadimage(&img_player_right[i], (LPCTSTR)path.c_str(), 0, 0, false);
    }
}
*/
void TryGenerateEnemy(std::vector<Enemy *> &enemy_list)
{
    const int INTERVAL = 100;
    static int counter = 0;
    if ((counter++) % INTERVAL == 0)
        enemy_list.push_back(new Enemy());
}

void UpdateBullets(std::vector<Bullet> &bullet_list, const Player &player)
{
    const double RADIAL_SPEED = 0.0045;
    const double TANGENT_SPEED = 0.0055;
    double radian_interval = 2 * 3.1415926 / bullet_list.size();
    POINT player_position = player.GetPosition();
    double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
    for (long long unsigned i = 0; i < bullet_list.size(); i++)
    {
        double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;
        bullet_list[i].postion.x = player_position.x + player.GetPlayerWidth() / 2 + (int)(radius * sin(radian));
        bullet_list[i].postion.y = player_position.y + player.GetPlayerHeight() / 2 + (int)(radius * cos(radian));
    }
}
void DrawPlayerScore(int score)
{
    static TCHAR text[64];
    _stprintf_s(text, _T("当前玩家得分：%d"), score);
    setbkmode(TRANSPARENT);
    settextcolor(RGB(255, 85, 185));
    outtextxy(10, 10, text);
}

void Teyvat_Run()
{
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    /*
        atlas_player_left = new Atlas(_T("../Teyvat Survivors/Teyvat Survivor (Advanced)/img/paimon_left_%d.png"), 6);
        atlas_player_right = new Atlas(_T("../Teyvat Survivors/Teyvat Survivor (Advanced)/img/paimon_right_%d.png"), 6);
        atlas_enemy_left = new Atlas(_T("../Teyvat Survivors/Teyvat Survivor (Advanced)/img/boar_left_%d.png"), 6);
        atlas_enemy_right = new Atlas(_T("../Teyvat Survivors/Teyvat Survivor (Advanced)/img/boar_right_%d.png"), 6);
    */

    // PlaySound(_T("../Teyvat Survivors/Teyvat Survivor (Advanced)/mus/bgm.wav"), NULL, SND_ASYNC | SND_LOOP);
    // mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
    // mciSendString(_T("play bgm repeat form 0"), NULL, 0, NULL);
    // mciSendString(_T("open mus/hit.mp3 alias hit"), NULL, 0, NULL);

    // mciSendString(_T("open ../Teyvat Survivors/Teyvat Survivor (Advanced)/mus/hit.wav alias hit"), NULL, 0, NULL);
    ExMessage msg;
    int score = 0;
    // L"../Teyvat Survivors/Teyvat Survivor (Advanced)/img/paimon_right_5.png"
    //
    IMAGE img_background;
    IMAGE img_menu;
    Player player;

    std::vector<Enemy *> enemy_list;
    std::vector<Bullet> bullet_list(3);
    loadimage(&img_menu, _T("../package/Teyvat Survivor (Advanced)/img/menu.png"));
    loadimage(&img_background, _T("../package/Teyvat Survivor (Advanced)/img/background.png"));

    RECT region_btn_start_game, region_btn_quit_game;
    region_btn_start_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
    region_btn_start_game.right = region_btn_start_game.left + BUTTON_WIDTH;
    region_btn_start_game.top = 430;
    region_btn_start_game.bottom = region_btn_start_game.top + BUTTON_HEIGHT;

    region_btn_quit_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
    region_btn_quit_game.right = region_btn_quit_game.left + BUTTON_WIDTH;
    region_btn_quit_game.top = 550;
    region_btn_quit_game.bottom = region_btn_quit_game.top + BUTTON_HEIGHT;

    StartGameButton btn_quit_game = StartGameButton(region_btn_start_game, _T("../package/Teyvat Survivor (Advanced)/img/ui_start_idle.png"), _T("../../package/Teyvat Survivor (Advanced)/img/ui_start_hovered.png"), _T("../../package/Teyvat Survivor (Advanced)/img/ui_start_pushed.png"));
    QuitGameButton btn_start_game = QuitGameButton(region_btn_quit_game, _T("../package/Teyvat Survivor (Advanced)/img/ui_quit_idle.png"), _T("../../package/Teyvat Survivor (Advanced)/img/ui_quit_hovered.png"), _T("../../package/Teyvat Survivor (Advanced)/img/ui_quit_pushed.png"));

    BeginBatchDraw();
    // LoadAnimation();
    while (running)
    {
        DWORD begin_time = GetTickCount();
        /*
        if (is_player_up)
            player_pos.y -= PLAYER_SPEED;
        if (is_player_down)
            player_pos.y += PLAYER_SPEED;
        if (is_player_left)
            player_pos.x -= PLAYER_SPEED;
        if (is_player_right)
            player_pos.x += PLAYER_SPEED;
        */
        while (peekmessage(&msg))
        {
            if (is_game_started)
                player.ProcessEvent(msg);
            else
            {
                btn_start_game.ProcessEvent(msg);
                btn_quit_game.ProcessEvent(msg);
            }
        }
        if (is_game_started)
        {
            player.Move();
            UpdateBullets(bullet_list, player);
            TryGenerateEnemy(enemy_list);
            for (Enemy *enemy : enemy_list)
            {
                enemy->Move(player);
            }

            for (Enemy *enemy : enemy_list)
            {
                if (enemy->CheckPlayerColetCollision(player))
                {
                    PlaySound(_T("../package/Teyvat Survivor (Advanced)/mus/hurt.wav"), NULL, SND_ASYNC);
                    MessageBox(GetHWnd(), _T("你被野猪创死了！"), _T("游戏结束"), MB_OK);
                    running = false;
                    break;
                }
            }

            for (Enemy *enemy : enemy_list)
            {
                for (const Bullet &bullet : bullet_list)
                {
                    if (enemy->CheckBulletColetCollision(bullet))
                    {
                        mciSendString(_T("play hit form 0"), NULL, 0, NULL);

                        // PlaySound(_T("../Teyvat Survivors/Teyvat Survivor (Advanced)/mus/hit.wav"), NULL, SND_ASYNC);
                        enemy->Hurt(bullet.GetDAMAGE());
                    }
                }
            }
            for (long long unsigned i = 0; i < enemy_list.size(); i++)
            {
                Enemy *enemy = enemy_list[i];
                if (!enemy->CheckAlive())
                {
                    score++;
                    std::swap(enemy_list[i], enemy_list.back());
                    enemy_list.pop_back();
                    delete enemy;
                }
            }
        }
        cleardevice();
        if (is_game_started)
        {
            putimage(0, 0, &img_background, SRCCOPY);
            player.Draw(1000 / 144);

            // putimage_alpha(player_pos.x, player_pos.y, &img_player_left[idx_current_anim]);
            for (Enemy *enemy : enemy_list)
            {
                enemy->Draw(1000 / 144);
            }
            for (const Bullet bullet : bullet_list)
            {
                bullet.Draw();
            }
            DrawPlayerScore(score);
        }
        else
        {
            putimage(0, 0, &img_menu);
            btn_start_game.Draw();
            btn_quit_game.Draw();
        }
        FlushBatchDraw();

        DWORD end_time = GetTickCount();
        DWORD delata_time = end_time - begin_time;

        if (delata_time < 1000 / 144)
            Sleep(1000 / 144 - delata_time);
    }
    delete atlas_player_left;
    delete atlas_player_right;
    delete atlas_enemy_left;
    delete atlas_enemy_right;
    EndBatchDraw();
}
