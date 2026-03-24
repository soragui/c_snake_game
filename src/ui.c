#include "ui.h"
#include "snake.h"
#include "food.h"
#include "obstacle.h"
#include "score.h"
#include "input.h"
#include "replay.h"
#include <ncurses.h>
#include <string.h>
#include <stdio.h>

// Static renderer instance
static renderer_t ncurses_renderer = {
    .init = ui_init,
    .cleanup = ui_cleanup,
    .clear_screen = ui_clear_screen,
    .draw_border = ui_draw_border,
    .draw_text = ui_draw_text,
    .refresh = ui_refresh_screen
};

// State handlers
static void start_screen_update(game_t* game);
static void start_screen_render(game_t* game);
static void start_screen_handle_input(game_t* game, int key);
static void start_screen_enter(game_t* game);
static void start_screen_exit(game_t* game);

static void game_screen_update(game_t* game);
static void game_screen_render(game_t* game);
static void game_screen_handle_input(game_t* game, int key);
static void game_screen_enter(game_t* game);
static void game_screen_exit(game_t* game);

static void game_over_screen_update(game_t* game);
static void game_over_screen_render(game_t* game);
static void game_over_screen_handle_input(game_t* game, int key);
static void game_over_screen_enter(game_t* game);
static void game_over_screen_exit(game_t* game);

static void pause_screen_update(game_t* game);
static void pause_screen_render(game_t* game);
static void pause_screen_handle_input(game_t* game, int key);
static void pause_screen_enter(game_t* game);
static void pause_screen_exit(game_t* game);

static state_handler_t start_handler = {
    .update = start_screen_update,
    .render = start_screen_render,
    .handle_input = start_screen_handle_input,
    .enter = start_screen_enter,
    .exit = start_screen_exit
};

static state_handler_t game_handler = {
    .update = game_screen_update,
    .render = game_screen_render,
    .handle_input = game_screen_handle_input,
    .enter = game_screen_enter,
    .exit = game_screen_exit
};

static state_handler_t game_over_handler = {
    .update = game_over_screen_update,
    .render = game_over_screen_render,
    .handle_input = game_over_screen_handle_input,
    .enter = game_over_screen_enter,
    .exit = game_over_screen_exit
};

static state_handler_t pause_handler = {
    .update = pause_screen_update,
    .render = pause_screen_render,
    .handle_input = pause_screen_handle_input,
    .enter = pause_screen_enter,
    .exit = pause_screen_exit
};

/******************************************************************************
 * @brief 初始化 UI 系统
 * 
 * 初始化 ncurses、设置颜色、初始化输入系统
 *****************************************************************************/
void ui_init(void) {
    initscr();
    ui_setup_colors();
    input_init();
}

/******************************************************************************
 * @brief 清理 UI 系统
 * 
 * 结束 ncurses 模式，恢复终端正常状态
 *****************************************************************************/
void ui_cleanup(void) {
    endwin();
}

/******************************************************************************
 * @brief 设置颜色对
 * 
 * 定义游戏使用的颜色对：蛇、食物、墙壁、UI、高亮
 *****************************************************************************/
void ui_setup_colors(void) {
    if (has_colors()) {
        start_color();

        // Define color pairs
        init_pair(COLOR_SNAKE, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_FOOD, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_WALL, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_UI, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_HIGHLIGHT, COLOR_YELLOW, COLOR_BLACK);
    }
}

/******************************************************************************
 * @brief 清屏
 *****************************************************************************/
void ui_clear_screen(void) {
    clear();
}

/******************************************************************************
 * @brief 刷新屏幕显示
 *****************************************************************************/
void ui_refresh_screen(void) {
    refresh();
}

/******************************************************************************
 * @brief 绘制游戏区域边框
 * 
 * 使用 '=' 绘制水平边框，'|' 绘制垂直边框
 * 
 * @param width 区域宽度
 * @param height 区域高度
 * @param offset_x X 偏移量
 * @param offset_y Y 偏移量
 *****************************************************************************/
void ui_draw_border(int width, int height, int offset_x, int offset_y) {
    attron(COLOR_PAIR(COLOR_WALL));

    // Draw horizontal borders
    for (int x = offset_x; x < offset_x + width; x++) {
        mvaddch(offset_y, x, '=');
        mvaddch(offset_y + height - 1, x, '=');
    }

    // Draw vertical borders
    for (int y = offset_y; y < offset_y + height; y++) {
        mvaddch(y, offset_x, '|');
        mvaddch(y, offset_x + width - 1, '|');
    }

    attroff(COLOR_PAIR(COLOR_WALL));
}

/******************************************************************************
 * @brief 在指定位置绘制文本
 * 
 * @param x X 坐标
 * @param y Y 坐标
 * @param text 要绘制的文本
 * @param color_pair 颜色对编号，0 表示不使用颜色
 *****************************************************************************/
void ui_draw_text(int x, int y, const char* text, int color_pair) {
    if (color_pair > 0) {
        attron(COLOR_PAIR(color_pair));
    }

    mvprintw(y, x, "%s", text);

    if (color_pair > 0) {
        attroff(COLOR_PAIR(color_pair));
    }
}

/******************************************************************************
 * @brief 在指定行居中绘制文本
 * 
 * @param y Y 坐标（行）
 * @param text 要绘制的文本
 * @param color_pair 颜色对编号
 *****************************************************************************/
void ui_draw_text_centered(int y, const char* text, int color_pair) {
    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);
    (void)term_height; // Suppress unused variable warning
    int x = (term_width - strlen(text)) / 2;
    ui_draw_text(x, y, text, color_pair);
}

/******************************************************************************
 * @brief 在指定位置绘制字符
 * 
 * @param x X 坐标
 * @param y Y 坐标
 * @param ch 要绘制的字符
 * @param color_pair 颜色对编号
 *****************************************************************************/
void ui_draw_char(int x, int y, char ch, int color_pair) {
    if (color_pair > 0) {
        attron(COLOR_PAIR(color_pair));
    }

    mvaddch(y, x, ch);

    if (color_pair > 0) {
        attroff(COLOR_PAIR(color_pair));
    }
}

/******************************************************************************
 * @brief 绘制蛇
 * 
 * 头部用 'O' 表示，身体用 '#' 表示
 * 
 * @param snake 蛇实例指针
 *****************************************************************************/
void ui_draw_snake(snake_t* snake) {
    if (!snake) return;

    snake_segment_t* current = snake->head;
    bool is_head = true;

    while (current) {
        char symbol = is_head ? 'O' : '#';
        ui_draw_char(current->position.x, current->position.y, symbol, COLOR_SNAKE);
        current = current->next;
        is_head = false;
    }
}

/******************************************************************************
 * @brief 绘制食物
 * 
 * 只绘制处于激活状态的食物
 * 
 * @param food 食物实例指针
 *****************************************************************************/
void ui_draw_food(food_t* food) {
    if (!food || !food->active) return;

    ui_draw_char(food->position.x, food->position.y,
                food->type->symbol, food->type->color_pair);
}

/******************************************************************************
 * @brief 绘制分数信息
 * 
 * 显示当前分数、历史最高分、当前等级
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void ui_draw_score(game_t* game) {
    if (!game) return;

    char score_text[64];
    char score2_text[64];
    char high_score_text[64];
    char level_text[32];

    snprintf(level_text, sizeof(level_text), "Level: %d", game->level);
    snprintf(high_score_text, sizeof(high_score_text), "High Score: %d", game->high_score);

    if (game->two_player) {
        // 2-player mode: show both players' scores
        snprintf(score_text, sizeof(score_text), "P1 Score: %d", game->score);
        snprintf(score2_text, sizeof(score2_text), "P2 Score: %d", game->score2);
        ui_draw_text(2, 1, score_text, COLOR_UI);
        ui_draw_text(2, 2, score2_text, COLOR_UI);
        ui_draw_text(2, 3, high_score_text, COLOR_UI);
        ui_draw_text(2, 4, level_text, COLOR_UI);
    } else {
        // 1-player mode
        snprintf(score_text, sizeof(score_text), "Score: %d", game->score);
        ui_draw_text(2, 1, score_text, COLOR_UI);
        ui_draw_text(2, 2, high_score_text, COLOR_UI);
        ui_draw_text(2, 3, level_text, COLOR_UI);
    }
}

/******************************************************************************
 * @brief 绘制开始屏幕
 * 
 * 显示游戏标题、难度选择菜单、操作提示
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void ui_render_start_screen(game_t* game) {
    if (!game) return;

    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);
    (void)term_width; // Suppress unused variable warning

    ui_clear_screen();

    // Draw title
    ui_draw_text_centered(term_height / 4, "SNAKE GAME", COLOR_HIGHLIGHT);
    ui_draw_text_centered(term_height / 4 + 2, "Select Difficulty Level:", COLOR_UI);

    // Draw level selection menu
    const char* levels[] = {
        "1. Easy (Slow)",
        "2. Medium",
        "3. Hard",
        "4. Very Hard",
        "5. Extreme (Fast)",
        "6. Easy [Wrap]",
        "7. Medium [Wrap]",
        "8. Hard [Wrap]",
        "9. Very Hard [Wrap]",
        "10. Extreme [Wrap]"
    };

    int start_y = term_height / 2 - 4;
    int num_levels = get_max_levels();
    for (int i = 0; i < num_levels && i < 10; i++) {
        int color = (i + 1 == game->selected_level) ? COLOR_HIGHLIGHT : COLOR_UI;
        ui_draw_text_centered(start_y + i, levels[i], color);
    }

    ui_draw_text_centered(term_height - 6, "Use Arrow Keys or WASD to select", COLOR_UI);
    ui_draw_text_centered(term_height - 5, "Press ENTER or SPACE to start", COLOR_UI);
    ui_draw_text_centered(term_height - 4, "Press 2 for 2-Player mode", COLOR_UI);
    ui_draw_text_centered(term_height - 3, "Press ESC or Q to quit", COLOR_UI);

    // Show current mode
    if (game->two_player) {
        ui_draw_text_centered(term_height / 4 + 4, "[2-PLAYER MODE]", COLOR_HIGHLIGHT);
    }

    ui_refresh_screen();
}

/******************************************************************************
 * @brief 绘制游戏屏幕
 * 
 * 绘制游戏边框、分数、蛇、食物、操作提示
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void ui_render_game_screen(game_t* game) {
    if (!game) return;

    ui_clear_screen();

    // Draw game border
    ui_draw_border(game->board_width, game->board_height,
                  game->board_offset_x, game->board_offset_y);

    // Draw score information
    ui_draw_score(game);

    // Draw obstacles (for levels with mazes)
    if (maze_has_obstacles(game->level)) {
        maze_t* maze = get_maze_for_level(game->level);
        if (maze) {
            obstacle_render(maze, game->board_offset_x, game->board_offset_y);
        }
    }

    // Draw game objects
    ui_draw_snake(game->snake);
    if (game->two_player && game->snake2) {
        ui_draw_snake(game->snake2);  // Render second snake (same color for now)
    }
    ui_draw_food(game->food);

    // Draw instructions
    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);
    (void)term_width; // Suppress unused variable warning

    ui_draw_text(2, term_height - 3, "Arrow Keys/WASD: Move", COLOR_UI);
    ui_draw_text(2, term_height - 2, "P/SPACE: Pause, ESC/Q: Menu", COLOR_UI);

    ui_refresh_screen();
}

/******************************************************************************
 * @brief 绘制游戏结束屏幕
 * 
 * 显示游戏结束标题、最终分数、历史最高分、重新开始选项
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void ui_render_game_over_screen(game_t* game) {
    if (!game) return;

    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);
    (void)term_width; // Suppress unused variable warning

    ui_clear_screen();

    // Draw game over title
    ui_draw_text_centered(term_height / 3, "GAME OVER", COLOR_HIGHLIGHT);

    if (game->two_player) {
        // 2-player mode: show both scores and winner
        char p1_score[64], p2_score[64], winner[64];
        snprintf(p1_score, sizeof(p1_score), "Player 1: %d", game->score);
        snprintf(p2_score, sizeof(p2_score), "Player 2: %d", game->score2);

        ui_draw_text_centered(term_height / 3 + 1, p1_score, COLOR_UI);
        ui_draw_text_centered(term_height / 3 + 2, p2_score, COLOR_UI);

        // Declare winner
        if (game->score > game->score2) {
            snprintf(winner, sizeof(winner), "Player 1 Wins!");
        } else if (game->score2 > game->score) {
            snprintf(winner, sizeof(winner), "Player 2 Wins!");
        } else {
            snprintf(winner, sizeof(winner), "It's a Tie!");
        }
        ui_draw_text_centered(term_height / 3 + 4, winner, COLOR_HIGHLIGHT);
    } else {
        // 1-player mode
        char final_score[64];
        char high_score[64];
        snprintf(final_score, sizeof(final_score), "Final Score: %d", game->score);
        snprintf(high_score, sizeof(high_score), "High Score: %d", game->high_score);

        ui_draw_text_centered(term_height / 3 + 2, final_score, COLOR_UI);
        ui_draw_text_centered(term_height / 3 + 3, high_score, COLOR_UI);

        if (game->score == game->high_score && game->score > 0) {
            ui_draw_text_centered(term_height / 3 + 5, "NEW HIGH SCORE!", COLOR_HIGHLIGHT);
        }
    }

    // Draw options
    ui_draw_text_centered(term_height - 6, "Press ENTER/SPACE/R to play again", COLOR_UI);
    ui_draw_text_centered(term_height - 5, "Press ESC/M for main menu", COLOR_UI);
    ui_draw_text_centered(term_height - 4, "Press Q to quit", COLOR_UI);

    ui_refresh_screen();
}

// State handler implementations
static void start_screen_update(game_t* game) {
    (void)game; // Suppress unused parameter warning
    // No update logic needed for start screen
}

static void start_screen_render(game_t* game) {
    ui_render_start_screen(game);
}

static void start_screen_handle_input(game_t* game, int key) {
    input_handle_start_screen(game, key);
}

static void start_screen_enter(game_t* game) {
    // Initialize selected level
    if (game->selected_level < 1) {
        game->selected_level = 1;
    }
}

static void start_screen_exit(game_t* game) {
    (void)game; // Suppress unused parameter warning
    // Nothing to clean up
}

static void game_screen_update(game_t* game) {
    if (!game || !game->snake) return;

    // Move player 1's snake
    if (game->snake->behavior && game->snake->behavior->move_snake) {
        game->snake->behavior->move_snake(game->snake, game);
    }

    // Move player 2's snake (if 2-player mode)
    if (game->two_player && game->snake2) {
        if (game->snake2->behavior && game->snake2->behavior->move_snake) {
            game->snake2->behavior->move_snake(game->snake2, game);
        }
    }

    // Check food collision for player 1
    if (game->food && game->food->active && game->snake) {
        point_t head_pos = snake_get_head_position(game->snake);
        if (food_is_at_position(game->food, head_pos)) {
            food_consume(game->food, game);
            food_spawn(game->food, game);
        }
    }

    // Check food collision for player 2 (if 2-player mode)
    if (game->two_player && game->snake2 && game->food && game->food->active) {
        point_t head_pos = snake_get_head_position(game->snake2);
        if (food_is_at_position(game->food, head_pos)) {
            // Player 2 eats food - add points to player 2's score
            if (game->snake2 && game->snake2->behavior && game->snake2->behavior->grow) {
                game->snake2->behavior->grow(game->snake2);
            }
            int points = score_calculate_food_points(game, game->food);
            game->score2 += points;
            food_spawn(game->food, game);
        }
    }

    // Check collisions for player 1
    bool p1_crashed = false;
    if (game->snake->behavior && game->snake->behavior->check_collision) {
        if (game->snake->behavior->check_collision(game->snake, game)) {
            p1_crashed = true;
        }
    }

    // Check collisions for player 2
    bool p2_crashed = false;
    if (game->two_player && game->snake2 && game->snake2->behavior && game->snake2->behavior->check_collision) {
        if (game->snake2->behavior->check_collision(game->snake2, game)) {
            p2_crashed = true;
        }
    }

    // Handle game over
    if (p1_crashed || p2_crashed) {
        game_set_state(game, STATE_GAME_OVER);
        // Save high score (player 1's score or player 2's in 2-player)
        int final_score = game->two_player ? game->score2 : game->score;
        if (game->two_player) {
            // In 2-player mode, save the winner's score
            final_score = (game->score > game->score2) ? game->score : game->score2;
        }
        if (score_is_new_high_score(final_score)) {
            score_save_high_score(final_score);
        }
    }
}

static void game_screen_render(game_t* game) {
    ui_render_game_screen(game);
}

// Static frame counter for replay recording
static int game_frame_counter = 0;

static void game_screen_handle_input(game_t* game, int key) {
    if (!game) return;

    // Record input for replay (only direction keys)
    if (input_is_direction_key(key) || input_is_direction_key_p2(key)) {
        replay_t* replay = replay_get_instance();
        if (replay && replay->is_recording) {
            replay_record_frame(replay, key, game_frame_counter++);
        }
    }

    // Handle player 1 input (arrow keys or configured bindings)
    if (input_is_direction_key(key)) {
        direction_t new_dir = input_key_to_direction(key);
        if (game->snake) {
            snake_set_direction(game->snake, new_dir);
        }
    }

    // Handle player 2 input (WASD keys)
    if (game->two_player && input_is_direction_key_p2(key)) {
        direction_t new_dir = input_key_to_direction_p2(key);
        if (game->snake2) {
            snake_set_direction(game->snake2, new_dir);
        }
    }

    // Handle pause/quit (both players can pause or quit)
    key_bindings_t* b = input_get_bindings();
    if (key == b->pause_1 || key == b->pause_2 || key == b->pause_3) {
        game_set_state(game, STATE_PAUSED);
    } else if (key == b->back || key == b->quit) {
        game_set_state(game, STATE_START_SCREEN);
    }
}

static void game_screen_enter(game_t* game) {
    (void)game; // Suppress unused parameter warning
    // Reset frame counter for replay
    game_frame_counter = 0;
}

static void game_screen_exit(game_t* game) {
    (void)game; // Suppress unused parameter warning
    // Stop recording and save replay
    replay_t* replay = replay_get_instance();
    if (replay->is_recording) {
        replay_stop_recording(replay);
        // Could save replay to file here if desired
        // replay_save_to_file(replay, "replays/latest.replay");
    }
}

static void game_over_screen_update(game_t* game) {
    (void)game; // Suppress unused parameter warning
    // No update logic needed for game over screen
}

static void game_over_screen_render(game_t* game) {
    ui_render_game_over_screen(game);
}

static void game_over_screen_handle_input(game_t* game, int key) {
    input_handle_game_over_screen(game, key);
}

static void game_over_screen_enter(game_t* game) {
    // Save high score when entering game over state
    if (score_is_new_high_score(game->score)) {
        score_save_high_score(game->score);
        game->high_score = game->score;
    }
}

static void game_over_screen_exit(game_t* game) {
    (void)game; // Suppress unused parameter warning
    // Nothing to clean up
}

// Pause screen implementations
static void pause_screen_update(game_t* game) {
    (void)game; // Suppress unused parameter warning
    // No update logic needed for pause screen
}

static void pause_screen_render(game_t* game) {
    if (!game) return;

    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);
    (void)term_width; // Suppress unused variable warning

    ui_clear_screen();

    // Draw game over title
    ui_draw_text_centered(term_height / 3, "PAUSED", COLOR_HIGHLIGHT);

    // Draw instructions
    ui_draw_text_centered(term_height / 2, "Press P/SPACE/ENTER to resume", COLOR_UI);
    ui_draw_text_centered(term_height / 2 + 1, "Press ESC/Q to quit to menu", COLOR_UI);

    ui_refresh_screen();
}

static void pause_screen_handle_input(game_t* game, int key) {
    input_handle_pause_screen(game, key);
}

static void pause_screen_enter(game_t* game) {
    (void)game; // Suppress unused parameter warning
    // Nothing special to do when entering pause state
}

static void pause_screen_exit(game_t* game) {
    (void)game; // Suppress unused parameter warning
    // Nothing to clean up
}

/******************************************************************************
 * @brief 获取开始屏幕状态处理器
 * 
 * @return state_handler_t* 开始屏幕处理器指针
 *****************************************************************************/
state_handler_t* get_start_screen_handler(void) {
    return &start_handler;
}

/******************************************************************************
 * @brief 获取游戏屏幕状态处理器
 * 
 * @return state_handler_t* 游戏屏幕处理器指针
 *****************************************************************************/
state_handler_t* get_game_screen_handler(void) {
    return &game_handler;
}

/******************************************************************************
 * @brief 获取游戏结束屏幕状态处理器
 *
 * @return state_handler_t* 游戏结束屏幕处理器指针
 *****************************************************************************/
state_handler_t* get_game_over_handler(void) {
    return &game_over_handler;
}

/******************************************************************************
 * @brief 获取暂停屏幕状态处理器
 *
 * @return state_handler_t* 暂停屏幕处理器指针
 *****************************************************************************/
state_handler_t* get_pause_screen_handler(void) {
    return &pause_handler;
}

/******************************************************************************
 * @brief 获取 ncurses 渲染器实例
 * 
 * @return renderer_t* ncurses 渲染器指针
 *****************************************************************************/
renderer_t* get_ncurses_renderer(void) {
    return &ncurses_renderer;
}
