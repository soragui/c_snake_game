#ifndef GAME_H
#define GAME_H

#include "utils.h"
#include <stdbool.h>

// Forward declarations
typedef struct snake snake_t;
typedef struct food food_t;
typedef struct game game_t;

// Game states
typedef enum {
    STATE_START_SCREEN,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_PAUSED,
    STATE_EXIT
} game_state_t;

// Function pointer interfaces
typedef struct {
    void (*update)(game_t* game);
    void (*render)(game_t* game);
    void (*handle_input)(game_t* game, int key);
    void (*enter)(game_t* game);
    void (*exit)(game_t* game);
} state_handler_t;

typedef struct {
    void (*move_snake)(snake_t* snake, game_t* game);
    bool (*check_collision)(snake_t* snake, game_t* game);
    void (*grow)(snake_t* snake);
} snake_behavior_t;

typedef struct {
    int value;
    char symbol;
    int color_pair;
    void (*on_eaten)(game_t* game, food_t* food);
} food_type_t;

typedef struct {
    void (*init)(void);
    void (*cleanup)(void);
    void (*clear_screen)(void);
    void (*draw_border)(int width, int height, int offset_x, int offset_y);
    void (*draw_text)(int x, int y, const char* text, int color_pair);
    void (*refresh)(void);
} renderer_t;

// Level configuration
typedef struct {
    int speed_delay;        // Milliseconds between moves
    int score_multiplier;   // Score multiplier for this level
    const char* name;       // Level name (e.g., "Easy", "Hard")
    snake_behavior_t* behavior;
    food_type_t* food_types;
    int num_food_types;
} level_config_t;

// Main game structure
struct game {
    game_state_t state;
    game_state_t next_state;

    snake_t* snake;
    food_t* food;

    int score;
    int high_score;
    int level;

    int board_width;
    int board_height;
    int board_offset_x;
    int board_offset_y;

    state_handler_t* current_handler;
    level_config_t* level_config;
    renderer_t* renderer;

    bool running;
    bool paused;

    // Menu state
    int selected_level;
};

// Game management functions
game_t* game_create(void);
void game_destroy(game_t* game);
void game_init(game_t* game);
void game_run(game_t* game);
void game_update(game_t* game);
void game_render(game_t* game);
void game_handle_input(game_t* game, int key);

// State management
void game_set_state(game_t* game, game_state_t new_state);
void game_change_level(game_t* game, int level);

// Level configuration
level_config_t* get_level_config(int level);
int get_max_levels(void);

// Game board utilities
void game_calculate_board_size(game_t* game);
bool game_is_point_in_bounds(game_t* game, point_t p);
bool game_is_point_on_border(game_t* game, point_t p);

#endif // GAME_H