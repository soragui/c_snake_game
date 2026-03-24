#include "input.h"
#include "snake.h"
#include "utils.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global key bindings instance
static key_bindings_t g_bindings;
static int bindings_initialized = 0;

/******************************************************************************
 * @brief 初始化输入系统
 *
 * 配置 ncurses 输入模式：
 * 1. 启用小键盘支持（方向键）
 * 2. 设置为非阻塞模式
 * 3. 禁用回显
 * 4. 隐藏光标
 * 5. 加载按键绑定
 *****************************************************************************/
void input_init(void) {
    // Enable keypad for arrow keys
    keypad(stdscr, TRUE);

    // Make getch() non-blocking
    nodelay(stdscr, TRUE);

    // Don't echo input
    noecho();

    // Hide cursor
    curs_set(0);

    // Initialize key bindings
    input_init_default_bindings(&g_bindings);
    input_load_bindings(&g_bindings);
    bindings_initialized = 1;
}

/******************************************************************************
 * @brief 初始化默认按键绑定
 *
 * @param bindings 按键绑定结构体指针
 *****************************************************************************/
void input_init_default_bindings(key_bindings_t* bindings) {
    if (!bindings) return;

    bindings->up_1 = KEY_UP;
    bindings->up_2 = 'w';
    bindings->down_1 = KEY_DOWN;
    bindings->down_2 = 's';
    bindings->left_1 = KEY_LEFT;
    bindings->left_2 = 'a';
    bindings->right_1 = KEY_RIGHT;
    bindings->right_2 = 'd';
    bindings->pause_1 = 'p';
    bindings->pause_2 = 'P';
    bindings->pause_3 = KEY_SPACE;
    bindings->select = KEY_ENTER;
    bindings->confirm = KEY_SPACE;
    bindings->back = KEY_ESC;
    bindings->quit = 'q';
}

/******************************************************************************
 * @brief 从文件加载按键绑定
 *
 * 配置文件格式：KEY=value 每行
 *
 * @param bindings 按键绑定结构体指针
 *****************************************************************************/
void input_load_bindings(key_bindings_t* bindings) {
    if (!bindings) return;

    FILE* file = fopen("data/keybindings.cfg", "r");
    if (!file) {
        // No config file, use defaults (already set)
        return;
    }

    char line[64];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;

        char key[32];
        int value;
        if (sscanf(line, "%31[^=]=%d", key, &value) == 2) {
            if (strcmp(key, "up_1") == 0) bindings->up_1 = value;
            else if (strcmp(key, "up_2") == 0) bindings->up_2 = value;
            else if (strcmp(key, "down_1") == 0) bindings->down_1 = value;
            else if (strcmp(key, "down_2") == 0) bindings->down_2 = value;
            else if (strcmp(key, "left_1") == 0) bindings->left_1 = value;
            else if (strcmp(key, "left_2") == 0) bindings->left_2 = value;
            else if (strcmp(key, "right_1") == 0) bindings->right_1 = value;
            else if (strcmp(key, "right_2") == 0) bindings->right_2 = value;
            else if (strcmp(key, "pause_1") == 0) bindings->pause_1 = value;
            else if (strcmp(key, "pause_2") == 0) bindings->pause_2 = value;
            else if (strcmp(key, "pause_3") == 0) bindings->pause_3 = value;
            else if (strcmp(key, "select") == 0) bindings->select = value;
            else if (strcmp(key, "confirm") == 0) bindings->confirm = value;
            else if (strcmp(key, "back") == 0) bindings->back = value;
            else if (strcmp(key, "quit") == 0) bindings->quit = value;
        }
    }

    fclose(file);
}

/******************************************************************************
 * @brief 保存按键绑定到文件
 *
 * @param bindings 按键绑定结构体指针
 *****************************************************************************/
void input_save_bindings(const key_bindings_t* bindings) {
    if (!bindings) return;

    FILE* file = fopen("data/keybindings.cfg", "w");
    if (!file) {
        return;
    }

    fprintf(file, "# Snake Game Key Bindings Configuration\n");
    fprintf(file, "# Format: KEY=VALUE (one per line)\n");
    fprintf(file, "# Comments start with #\n\n");
    fprintf(file, "up_1=%d\n", bindings->up_1);
    fprintf(file, "up_2=%d\n", bindings->up_2);
    fprintf(file, "down_1=%d\n", bindings->down_1);
    fprintf(file, "down_2=%d\n", bindings->down_2);
    fprintf(file, "left_1=%d\n", bindings->left_1);
    fprintf(file, "left_2=%d\n", bindings->left_2);
    fprintf(file, "right_1=%d\n", bindings->right_1);
    fprintf(file, "right_2=%d\n", bindings->right_2);
    fprintf(file, "pause_1=%d\n", bindings->pause_1);
    fprintf(file, "pause_2=%d\n", bindings->pause_2);
    fprintf(file, "pause_3=%d\n", bindings->pause_3);
    fprintf(file, "select=%d\n", bindings->select);
    fprintf(file, "confirm=%d\n", bindings->confirm);
    fprintf(file, "back=%d\n", bindings->back);
    fprintf(file, "quit=%d\n", bindings->quit);

    fclose(file);
}

/******************************************************************************
 * @brief 获取全局按键绑定实例
 *
 * @return key_bindings_t* 按键绑定结构体指针
 *****************************************************************************/
key_bindings_t* input_get_bindings(void) {
    if (!bindings_initialized) {
        input_init_default_bindings(&g_bindings);
        bindings_initialized = 1;
    }
    return &g_bindings;
}

/******************************************************************************
 * @brief 获取按键输入
 * 
 * 非阻塞方式读取按键，立即返回
 * 
 * @return int 按键值，无输入返回 ERR
 *****************************************************************************/
int input_get_key(void) {
    return getch();
}

/******************************************************************************
 * @brief 检查按键是否为方向键
 *
 * 使用配置的按键绑定检查
 *
 * @param key 按键值
 * @return bool 是方向键返回 true，否则返回 false
 *****************************************************************************/
bool input_is_direction_key(int key) {
    key_bindings_t* b = input_get_bindings();
    return (key == b->up_1 || key == b->up_2 ||
            key == b->down_1 || key == b->down_2 ||
            key == b->left_1 || key == b->left_2 ||
            key == b->right_1 || key == b->right_2);
}

/******************************************************************************
 * @brief 将按键转换为方向
 *
 * 使用配置的按键绑定进行映射
 *
 * @param key 按键值
 * @return direction_t 对应的方向
 *****************************************************************************/
direction_t input_key_to_direction(int key) {
    key_bindings_t* b = input_get_bindings();

    if (key == b->up_1 || key == b->up_2) return DIR_UP;
    if (key == b->down_1 || key == b->down_2) return DIR_DOWN;
    if (key == b->left_1 || key == b->left_2) return DIR_LEFT;
    if (key == b->right_1 || key == b->right_2) return DIR_RIGHT;

    return DIR_UP; // Default fallback
}

/******************************************************************************
 * @brief 检查按键是否为玩家 2 的方向键
 *
 * 玩家 2 使用 WASD 键：
 * - W -> DIR_UP
 * - S -> DIR_DOWN
 * - A -> DIR_LEFT
 * - D -> DIR_RIGHT
 *
 * @param key 按键值
 * @return bool 是方向键返回 true，否则返回 false
 *****************************************************************************/
bool input_is_direction_key_p2(int key) {
    return (key == 'w' || key == 'W' ||
            key == 's' || key == 'S' ||
            key == 'a' || key == 'A' ||
            key == 'd' || key == 'D');
}

/******************************************************************************
 * @brief 将玩家 2 的按键转换为方向
 *
 * @param key 按键值
 * @return direction_t 对应的方向
 *****************************************************************************/
direction_t input_key_to_direction_p2(int key) {
    switch (key) {
        case 'w':
        case 'W':
            return DIR_UP;

        case 's':
        case 'S':
            return DIR_DOWN;

        case 'a':
        case 'A':
            return DIR_LEFT;

        case 'd':
        case 'D':
            return DIR_RIGHT;

        default:
            return DIR_UP;
    }
}

/******************************************************************************
 * @brief 处理开始屏幕的输入
 * 
 * 功能：
 * - 上/下键选择难度等级
 * - ENTER/SPACE 开始游戏
 * - ESC/Q 退出
 * 
 * @param game 游戏实例指针
 * @param key 按键值
 *****************************************************************************/
void input_handle_start_screen(game_t* game, int key) {
    if (!game) return;

    key_bindings_t* b = input_get_bindings();

    if (key == b->up_1 || key == b->up_2) {
        if (game->selected_level > 1) {
            game->selected_level--;
        }
    } else if (key == b->down_1 || key == b->down_2) {
        if (game->selected_level < get_max_levels()) {
            game->selected_level++;
        }
    } else if (key == b->select || key == b->confirm) {
        game_change_level(game, game->selected_level);
        game_set_state(game, STATE_PLAYING);
    } else if (key == '2') {
        // Toggle 2-player mode
        game->two_player = !game->two_player;
    } else if (key == b->back || key == b->quit) {
        game_set_state(game, STATE_EXIT);
    }
}

/******************************************************************************
 * @brief 处理游戏屏幕的输入
 * 
 * 功能：
 * - 方向键控制蛇移动
 * - P/SPACE 暂停游戏
 * - ESC/Q 返回主菜单
 * 
 * @param game 游戏实例指针
 * @param key 按键值
 *****************************************************************************/
void input_handle_game_screen(game_t* game, int key) {
    if (!game) return;

    key_bindings_t* b = input_get_bindings();

    if (input_is_direction_key(key)) {
        direction_t new_dir = input_key_to_direction(key);
        if (game->snake) {
            snake_set_direction(game->snake, new_dir);
        }
    } else if (key == b->pause_1 || key == b->pause_2 || key == b->pause_3) {
        game_set_state(game, STATE_PAUSED);
    } else if (key == b->back || key == b->quit) {
        game_set_state(game, STATE_START_SCREEN);
    }
}

/******************************************************************************
 * @brief 处理游戏结束屏幕的输入
 *
 * 功能：
 * - ENTER/SPACE/R 重新开始
 * - ESC/M 返回主菜单
 * - Q 退出游戏
 *
 * @param game 游戏实例指针
 * @param key 按键值
 *****************************************************************************/
void input_handle_game_over_screen(game_t* game, int key) {
    if (!game) return;

    key_bindings_t* b = input_get_bindings();

    if (key == b->select || key == b->confirm || key == 'r' || key == 'R') {
        // Restart game with same level
        game_change_level(game, game->level);
        game_set_state(game, STATE_PLAYING);
    } else if (key == b->back || key == 'm' || key == 'M') {
        game_set_state(game, STATE_START_SCREEN);
    } else if (key == b->quit || key == 'Q') {
        game_set_state(game, STATE_EXIT);
    }
}

/******************************************************************************
 * @brief 处理暂停屏幕的输入
 *
 * 功能：
 * - P/SPACE/ENTER 恢复游戏
 * - ESC/Q 返回主菜单
 *
 * @param game 游戏实例指针
 * @param key 按键值
 *****************************************************************************/
void input_handle_pause_screen(game_t* game, int key) {
    if (!game) return;

    key_bindings_t* b = input_get_bindings();

    if (key == b->pause_1 || key == b->pause_2 || key == b->pause_3 ||
        key == b->select || key == b->confirm) {
        game_set_state(game, STATE_PLAYING);
    } else if (key == b->back || key == b->quit) {
        game_set_state(game, STATE_START_SCREEN);
    }
}
