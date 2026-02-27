#include "input.h"
#include "snake.h"
#include <ncurses.h>

/******************************************************************************
 * @brief 初始化输入系统
 * 
 * 配置 ncurses 输入模式：
 * 1. 启用小键盘支持（方向键）
 * 2. 设置为非阻塞模式
 * 3. 禁用回显
 * 4. 隐藏光标
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
 * 支持的键：方向键、WASD（大小写）
 * 
 * @param key 按键值
 * @return bool 是方向键返回 true，否则返回 false
 *****************************************************************************/
bool input_is_direction_key(int key) {
    return (key == KEY_UP || key == KEY_DOWN ||
            key == KEY_LEFT || key == KEY_RIGHT ||
            key == 'w' || key == 'W' ||
            key == 's' || key == 'S' ||
            key == 'a' || key == 'A' ||
            key == 'd' || key == 'D');
}

/******************************************************************************
 * @brief 将按键转换为方向
 * 
 * 按键映射：
 * - KEY_UP/w/W -> DIR_UP
 * - KEY_DOWN/s/S -> DIR_DOWN
 * - KEY_LEFT/a/A -> DIR_LEFT
 * - KEY_RIGHT/d/D -> DIR_RIGHT
 * 
 * @param key 按键值
 * @return direction_t 对应的方向
 *****************************************************************************/
direction_t input_key_to_direction(int key) {
    switch (key) {
        case KEY_UP:
        case 'w':
        case 'W':
            return DIR_UP;

        case KEY_DOWN:
        case 's':
        case 'S':
            return DIR_DOWN;

        case KEY_LEFT:
        case 'a':
        case 'A':
            return DIR_LEFT;

        case KEY_RIGHT:
        case 'd':
        case 'D':
            return DIR_RIGHT;

        default:
            return DIR_UP; // Default fallback
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

    switch (key) {
        case KEY_UP:
        case 'w':
        case 'W':
            if (game->selected_level > 1) {
                game->selected_level--;
            }
            break;

        case KEY_DOWN:
        case 's':
        case 'S':
            if (game->selected_level < get_max_levels()) {
                game->selected_level++;
            }
            break;

        case KEY_ENTER:
        case KEY_SPACE:
            game_change_level(game, game->selected_level);
            game_set_state(game, STATE_PLAYING);
            break;

        case KEY_ESC:
        case 'q':
        case 'Q':
            game_set_state(game, STATE_EXIT);
            break;
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

    if (input_is_direction_key(key)) {
        direction_t new_dir = input_key_to_direction(key);
        if (game->snake) {
            snake_set_direction(game->snake, new_dir);
        }
    } else {
        switch (key) {
            case 'p':
            case 'P':
            case KEY_SPACE:
                game_set_state(game, STATE_PAUSED);
                break;

            case KEY_ESC:
            case 'q':
            case 'Q':
                game_set_state(game, STATE_START_SCREEN);
                break;
        }
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

    switch (key) {
        case KEY_ENTER:
        case KEY_SPACE:
        case 'r':
        case 'R':
            // Restart game with same level
            game_change_level(game, game->level);
            game_set_state(game, STATE_PLAYING);
            break;

        case KEY_ESC:
        case 'm':
        case 'M':
            game_set_state(game, STATE_START_SCREEN);
            break;

        case 'q':
        case 'Q':
            game_set_state(game, STATE_EXIT);
            break;
    }
}
