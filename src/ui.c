#include "ui.h"
#include "snake.h"
#include "food.h"
#include "score.h"
#include "input.h"
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

void ui_init(void) {
    initscr();
    ui_setup_colors();
    input_init();
}

void ui_cleanup(void) {
    endwin();
}

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

void ui_clear_screen(void) {
    clear();
}

void ui_refresh_screen(void) {
    refresh();
}

void ui_draw_border(int width, int height, int offset_x, int offset_y) {
    attron(COLOR_PAIR(COLOR_WALL));

    // Draw horizontal borders
    for (int x = offset_x; x < offset_x + width; x++) {
        mvaddch(offset_y, x, '#');
        mvaddch(offset_y + height - 1, x, '#');
    }

    // Draw vertical borders
    for (int y = offset_y; y < offset_y + height; y++) {
        mvaddch(y, offset_x, '#');
        mvaddch(y, offset_x + width - 1, '#');
    }

    attroff(COLOR_PAIR(COLOR_WALL));
}

void ui_draw_text(int x, int y, const char* text, int color_pair) {
    if (color_pair > 0) {
        attron(COLOR_PAIR(color_pair));
    }

    mvprintw(y, x, "%s", text);

    if (color_pair > 0) {
        attroff(COLOR_PAIR(color_pair));
    }
}

void ui_draw_text_centered(int y, const char* text, int color_pair) {
    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);
    (void)term_height; // Suppress unused variable warning
    int x = (term_width - strlen(text)) / 2;
    ui_draw_text(x, y, text, color_pair);
}

void ui_draw_char(int x, int y, char ch, int color_pair) {
    if (color_pair > 0) {
        attron(COLOR_PAIR(color_pair));
    }

    mvaddch(y, x, ch);

    if (color_pair > 0) {
        attroff(COLOR_PAIR(color_pair));
    }
}

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

void ui_draw_food(food_t* food) {
    if (!food || !food->active) return;

    ui_draw_char(food->position.x, food->position.y,
                food->type->symbol, food->type->color_pair);
}

void ui_draw_score(game_t* game) {
    if (!game) return;

    char score_text[64];
    char high_score_text[64];
    char level_text[32];

    snprintf(score_text, sizeof(score_text), "Score: %d", game->score);
    snprintf(high_score_text, sizeof(high_score_text), "High Score: %d", game->high_score);
    snprintf(level_text, sizeof(level_text), "Level: %d", game->level);

    ui_draw_text(2, 1, score_text, COLOR_UI);
    ui_draw_text(2, 2, high_score_text, COLOR_UI);
    ui_draw_text(2, 3, level_text, COLOR_UI);
}

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
        "5. Extreme (Fast)"
    };

    int start_y = term_height / 2 - 2;
    for (int i = 0; i < 5; i++) {
        int color = (i + 1 == game->selected_level) ? COLOR_HIGHLIGHT : COLOR_UI;
        ui_draw_text_centered(start_y + i, levels[i], color);
    }

    ui_draw_text_centered(term_height - 6, "Use Arrow Keys or WASD to select", COLOR_UI);
    ui_draw_text_centered(term_height - 5, "Press ENTER or SPACE to start", COLOR_UI);
    ui_draw_text_centered(term_height - 4, "Press ESC or Q to quit", COLOR_UI);

    ui_refresh_screen();
}

void ui_render_game_screen(game_t* game) {
    if (!game) return;

    ui_clear_screen();

    // Draw game border
    ui_draw_border(game->board_width, game->board_height,
                  game->board_offset_x, game->board_offset_y);

    // Draw score information
    ui_draw_score(game);

    // Draw game objects
    ui_draw_snake(game->snake);
    ui_draw_food(game->food);

    // Draw instructions
    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);
    (void)term_width; // Suppress unused variable warning

    ui_draw_text(2, term_height - 3, "Arrow Keys/WASD: Move", COLOR_UI);
    ui_draw_text(2, term_height - 2, "P/SPACE: Pause, ESC/Q: Menu", COLOR_UI);

    ui_refresh_screen();
}

void ui_render_game_over_screen(game_t* game) {
    if (!game) return;

    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);
    (void)term_width; // Suppress unused variable warning

    ui_clear_screen();

    // Draw game over title
    ui_draw_text_centered(term_height / 3, "GAME OVER", COLOR_HIGHLIGHT);

    // Draw scores
    char final_score[64];
    char high_score[64];
    snprintf(final_score, sizeof(final_score), "Final Score: %d", game->score);
    snprintf(high_score, sizeof(high_score), "High Score: %d", game->high_score);

    ui_draw_text_centered(term_height / 3 + 2, final_score, COLOR_UI);
    ui_draw_text_centered(term_height / 3 + 3, high_score, COLOR_UI);

    if (game->score == game->high_score && game->score > 0) {
        ui_draw_text_centered(term_height / 3 + 5, "NEW HIGH SCORE!", COLOR_HIGHLIGHT);
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

    // Move snake
    if (game->snake->behavior && game->snake->behavior->move_snake) {
        game->snake->behavior->move_snake(game->snake, game);
    }

    // Check food collision
    if (game->food && game->food->active) {
        point_t head_pos = snake_get_head_position(game->snake);
        if (food_is_at_position(game->food, head_pos)) {
            food_consume(game->food, game);
            food_spawn(game->food, game);
        }
    }

    // Check collisions
    if (game->snake->behavior && game->snake->behavior->check_collision) {
        if (game->snake->behavior->check_collision(game->snake, game)) {
            game_set_state(game, STATE_GAME_OVER);
            // Save high score if it's a new record
            if (score_is_new_high_score(game->score)) {
                score_save_high_score(game->score);
            }
        }
    }
}

static void game_screen_render(game_t* game) {
    ui_render_game_screen(game);
}

static void game_screen_handle_input(game_t* game, int key) {
    input_handle_game_screen(game, key);
}

static void game_screen_enter(game_t* game) {
    (void)game; // Suppress unused parameter warning
    // Nothing special to do when entering game state
}

static void game_screen_exit(game_t* game) {
    (void)game; // Suppress unused parameter warning
    // Nothing to clean up
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

// Public getter functions for state handlers
state_handler_t* get_start_screen_handler(void) {
    return &start_handler;
}

state_handler_t* get_game_screen_handler(void) {
    return &game_handler;
}

state_handler_t* get_game_over_handler(void) {
    return &game_over_handler;
}

renderer_t* get_ncurses_renderer(void) {
    return &ncurses_renderer;
}