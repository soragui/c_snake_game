#include "game.h"
#include "snake.h"
#include "food.h"
#include "score.h"
#include "ui.h"
#include "input.h"
#include "utils.h"
#include "replay.h"
#include <stdlib.h>
#include <stdio.h>

// Level configurations
// Classic levels (normal behavior)
static level_config_t level_configs_classic[] = {
    {200, 1, "Easy", NULL, NULL, 1},
    {150, 2, "Medium", NULL, NULL, 1},
    {100, 3, "Hard", NULL, NULL, 1},
    {75,  4, "Very Hard", NULL, NULL, 1},
    {50,  5, "Extreme", NULL, NULL, 1}
};

// Wraparound levels (wall-crossing behavior)
static level_config_t level_configs_wraparound[] = {
    {200, 1, "Easy (Wrap)", NULL, NULL, 1},
    {150, 2, "Medium (Wrap)", NULL, NULL, 1},
    {100, 3, "Hard (Wrap)", NULL, NULL, 1},
    {75,  4, "Very Hard (Wrap)", NULL, NULL, 1},
    {50,  5, "Extreme (Wrap)", NULL, NULL, 1}
};

// All levels (for selection)
static level_config_t* all_level_configs[] = {
    &level_configs_classic[0],
    &level_configs_classic[1],
    &level_configs_classic[2],
    &level_configs_classic[3],
    &level_configs_classic[4],
    &level_configs_wraparound[0],
    &level_configs_wraparound[1],
    &level_configs_wraparound[2],
    &level_configs_wraparound[3],
    &level_configs_wraparound[4]
};

static int num_levels = sizeof(all_level_configs) / sizeof(all_level_configs[0]);

/******************************************************************************
 * @brief 创建游戏实例
 * 
 * 分配并初始化 game_t 结构体，设置所有初始值为默认状态
 * 
 * @return game_t* 游戏实例指针，失败返回 NULL
 *****************************************************************************/
game_t* game_create(void) {
    game_t* game = malloc(sizeof(game_t));
    if (!game) return NULL;

    // Initialize game state
    game->state = STATE_START_SCREEN;
    game->next_state = STATE_START_SCREEN;
    game->snake = NULL;
    game->snake2 = NULL;
    game->food = NULL;
    game->score = 0;
    game->score2 = 0;
    game->high_score = 0;
    game->level = 1;
    game->selected_level = 1;
    game->board_width = 0;
    game->board_height = 0;
    game->board_offset_x = 0;
    game->board_offset_y = 0;
    game->current_handler = NULL;
    game->level_config = NULL;
    game->renderer = NULL;
    game->running = true;
    game->paused = false;
    game->two_player = false;  // Default to 1-player mode

    return game;
}

/******************************************************************************
 * @brief 销毁游戏实例并释放资源
 * 
 * 释放蛇、食物等游戏对象，最后释放 game 结构体本身
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void game_destroy(game_t* game) {
    if (!game) return;

    if (game->snake) {
        snake_destroy(game->snake);
    }

    if (game->snake2) {
        snake_destroy(game->snake2);
    }

    if (game->food) {
        food_destroy(game->food);
    }

    free(game);
}

/******************************************************************************
 * @brief 初始化游戏系统
 * 
 * 初始化随机数生成器、分数系统、渲染器，并设置初始状态处理器
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void game_init(game_t* game) {
    if (!game) return;

    // Initialize random number generator
    init_random();

    // Initialize score system
    score_init(game);

    // Set up renderer
    game->renderer = get_ncurses_renderer();
    if (game->renderer && game->renderer->init) {
        game->renderer->init();
    }

    // Check if terminal size is adequate
    if (!is_terminal_size_valid()) {
        printf("Terminal too small! Minimum size: %dx%d\n",
               MIN_TERMINAL_WIDTH, MIN_TERMINAL_HEIGHT);
        game->running = false;
        return;
    }

    // Calculate initial board size
    game_calculate_board_size(game);

    // Set initial state handler
    game->current_handler = get_start_screen_handler();
    if (game->current_handler && game->current_handler->enter) {
        game->current_handler->enter(game);
    }
}

/******************************************************************************
 * @brief 运行游戏主循环
 * 
 * 主循环流程:
 * 1. 处理用户输入
 * 2. 处理状态转换
 * 3. 根据时间间隔更新游戏逻辑
 * 4. 渲染画面
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void game_run(game_t* game) {
    if (!game) return;

    int last_update_time = 0;
    int current_time = 0;

    while (game->running) {
        // Handle input
        int key = input_get_key();
        if (key != ERR) {
            game_handle_input(game, key);
        }

        // Handle state transitions
        if (game->state != game->next_state) {
            if (game->current_handler && game->current_handler->exit) {
                game->current_handler->exit(game);
            }

            game->state = game->next_state;

            switch (game->state) {
                case STATE_START_SCREEN:
                    game->current_handler = get_start_screen_handler();
                    break;
                case STATE_PLAYING:
                    game->current_handler = get_game_screen_handler();
                    break;
                case STATE_GAME_OVER:
                    game->current_handler = get_game_over_handler();
                    break;
                case STATE_PAUSED:
                    game->current_handler = get_pause_screen_handler();
                    break;
                case STATE_EXIT:
                    game->running = false;
                    continue;
            }

            if (game->current_handler && game->current_handler->enter) {
                game->current_handler->enter(game);
            }
        }

        // Update game logic based on timing
        current_time++; // Simple frame counter
        int update_interval = game->level_config ? game->level_config->speed_delay / 10 : 20;

        if (game->state == STATE_PLAYING && !game->paused &&
            current_time - last_update_time >= update_interval) {
            game_update(game);
            last_update_time = current_time;
        }

        // Render
        game_render(game);

        // Small delay to prevent excessive CPU usage
        sleep_ms(10);
    }

    // Cleanup
    if (game->renderer && game->renderer->cleanup) {
        game->renderer->cleanup();
    }
}

/******************************************************************************
 * @brief 更新游戏逻辑
 * 
 * 调用当前状态处理器的 update 函数来更新游戏状态
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void game_update(game_t* game) {
    if (!game || !game->current_handler) return;

    if (game->current_handler->update) {
        game->current_handler->update(game);
    }
}

/******************************************************************************
 * @brief 渲染游戏画面
 * 
 * 调用当前状态处理器的 render 函数来绘制游戏画面
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void game_render(game_t* game) {
    if (!game || !game->current_handler) return;

    if (game->current_handler->render) {
        game->current_handler->render(game);
    }
}

/******************************************************************************
 * @brief 处理用户输入
 * 
 * 处理暂停/取消暂停逻辑，然后将输入委托给当前状态处理器
 * 
 * @param game 游戏实例指针
 * @param key 按键值
 *****************************************************************************/
void game_handle_input(game_t* game, int key) {
    if (!game || !game->current_handler) return;

    // Handle pause/unpause
    if (game->state == STATE_PAUSED) {
        if (key == 'p' || key == 'P' || key == ' ') {
            game_set_state(game, STATE_PLAYING);
        } else if (key == 27 || key == 'q' || key == 'Q') { // ESC or Q
            game_set_state(game, STATE_START_SCREEN);
        }
        return;
    }

    if (game->current_handler->handle_input) {
        game->current_handler->handle_input(game, key);
    }
}

/******************************************************************************
 * @brief 设置游戏状态
 * 
 * 设置下一个游戏状态，状态转换将在下一帧执行
 * 
 * @param game 游戏实例指针
 * @param new_state 新的游戏状态
 *****************************************************************************/
void game_set_state(game_t* game, game_state_t new_state) {
    if (!game) return;
    game->next_state = new_state;
}

/******************************************************************************
 * @brief 切换游戏难度等级
 * 
 * 更改难度等级并重置游戏元素（蛇、食物、分数）
 * 
 * @param game 游戏实例指针
 * @param level 难度等级 (1-5)
 *****************************************************************************/
void game_change_level(game_t* game, int level) {
    if (!game || level < 1 || level > get_max_levels()) return;

    game->level = level;
    game->level_config = get_level_config(level);

    // Reset game elements
    score_reset(game);
    game->score2 = 0;

    // Destroy existing snakes and food
    if (game->snake) {
        snake_destroy(game->snake);
        game->snake = NULL;
    }
    if (game->snake2) {
        snake_destroy(game->snake2);
        game->snake2 = NULL;
    }
    if (game->food) {
        food_destroy(game->food);
        game->food = NULL;
    }

    // Recalculate board size in case terminal was resized
    game_calculate_board_size(game);

    // Create player 1 snake at center-left
    int start_x1 = game->board_offset_x + game->board_width / 3;
    int start_y1 = game->board_offset_y + game->board_height / 2;
    game->snake = snake_create(start_x1, start_y1, DIR_RIGHT);

    // Set snake behavior based on level config
    if (game->snake && game->level_config && game->level_config->behavior) {
        game->snake->behavior = game->level_config->behavior;
    }

    // Create player 2 snake for 2-player mode at center-right
    if (game->two_player) {
        int start_x2 = game->board_offset_x + 2 * game->board_width / 3;
        int start_y2 = game->board_offset_y + game->board_height / 2;
        game->snake2 = snake_create(start_x2, start_y2, DIR_LEFT);

        if (game->snake2 && game->level_config && game->level_config->behavior) {
            game->snake2->behavior = game->level_config->behavior;
        }
    }

    // Create and spawn food
    game->food = food_create();
    if (game->food) {
        food_spawn(game->food, game);
    }

    // Start recording replay
    replay_t* replay = replay_get_instance();
    replay_start_recording(replay, level, game->two_player);
}

/******************************************************************************
 * @brief 获取难度等级配置
 *
 * 根据等级返回对应的配置结构体：
 * - Level 1-5: Classic modes (normal behavior)
 * - Level 6-10: Wraparound modes (wall-crossing behavior)
 *
 * @param level 难度等级 (1-10)
 * @return level_config_t* 等级配置指针
 *****************************************************************************/
level_config_t* get_level_config(int level) {
    if (level < 1 || level > get_max_levels()) {
        return all_level_configs[0]; // Return first level as default
    }

    level_config_t* config = all_level_configs[level - 1];

    // Set behavior based on level (1-5: classic, 6-10: wraparound)
    if (level <= 5) {
        config->behavior = get_normal_snake_behavior();
    } else {
        config->behavior = get_wraparound_snake_behavior();
    }

    // Set food types
    config->food_types = get_food_types_for_level(level, &config->num_food_types);

    return config;
}

/******************************************************************************
 * @brief 获取最大难度等级数
 *
 * @return int 最大等级数 (10 = 5 classic + 5 wraparound)
 *****************************************************************************/
int get_max_levels(void) {
    return num_levels;
}

/******************************************************************************
 * @brief 计算游戏区域尺寸
 * 
 * 根据终端尺寸计算游戏区域的大小和位置，预留 UI 显示空间
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void game_calculate_board_size(game_t* game) {
    if (!game) return;

    int term_width, term_height;
    get_terminal_size(&term_width, &term_height);

    // Reserve space for UI elements
    int ui_width = 30;  // Space for score display
    int ui_height = 6;  // Space for instructions

    // Calculate board dimensions
    game->board_width = term_width - ui_width;
    game->board_height = term_height - ui_height;

    // Ensure minimum board size
    if (game->board_width < 20) game->board_width = 20;
    if (game->board_height < 15) game->board_height = 15;

    // Calculate board position (centered in available space)
    game->board_offset_x = ui_width;
    game->board_offset_y = 2;

    // Ensure board fits in terminal
    if (game->board_offset_x + game->board_width > term_width) {
        game->board_width = term_width - game->board_offset_x - 1;
    }

    if (game->board_offset_y + game->board_height > term_height - 4) {
        game->board_height = term_height - game->board_offset_y - 4;
    }
}

/******************************************************************************
 * @brief 检查点是否在游戏区域边界内
 * 
 * @param game 游戏实例指针
 * @param p 要检查的点
 * @return bool 在边界内返回 true，否则返回 false
 *****************************************************************************/
bool game_is_point_in_bounds(game_t* game, point_t p) {
    if (!game) return false;

    return (p.x >= game->board_offset_x &&
            p.x < game->board_offset_x + game->board_width &&
            p.y >= game->board_offset_y &&
            p.y < game->board_offset_y + game->board_height);
}

/******************************************************************************
 * @brief 检查点是否在游戏区域边框上
 * 
 * @param game 游戏实例指针
 * @param p 要检查的点
 * @return bool 在边框上返回 true，否则返回 false
 *****************************************************************************/
bool game_is_point_on_border(game_t* game, point_t p) {
    if (!game) return false;

    return (p.x == game->board_offset_x ||
            p.x == game->board_offset_x + game->board_width - 1 ||
            p.y == game->board_offset_y ||
            p.y == game->board_offset_y + game->board_height - 1);
}
