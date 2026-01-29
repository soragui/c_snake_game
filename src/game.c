#include "game.h"
#include "snake.h"
#include "food.h"
#include "score.h"
#include "ui.h"
#include "input.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

// Level configurations
static level_config_t level_configs[] = {
    {200, 1, "Easy", NULL, NULL, 1},        // Level 1
    {150, 2, "Medium", NULL, NULL, 1},      // Level 2
    {100, 3, "Hard", NULL, NULL, 1},        // Level 3
    {75,  4, "Very Hard", NULL, NULL, 1},   // Level 4
    {50,  5, "Extreme", NULL, NULL, 1}      // Level 5
};

game_t* game_create(void) {
    game_t* game = malloc(sizeof(game_t));
    if (!game) return NULL;

    // Initialize game state
    game->state = STATE_START_SCREEN;
    game->next_state = STATE_START_SCREEN;
    game->snake = NULL;
    game->food = NULL;
    game->score = 0;
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

    return game;
}

void game_destroy(game_t* game) {
    if (!game) return;

    if (game->snake) {
        snake_destroy(game->snake);
    }

    if (game->food) {
        food_destroy(game->food);
    }

    free(game);
}

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
                    // Keep current handler but stop updating
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

void game_update(game_t* game) {
    if (!game || !game->current_handler) return;

    if (game->current_handler->update) {
        game->current_handler->update(game);
    }
}

void game_render(game_t* game) {
    if (!game || !game->current_handler) return;

    if (game->current_handler->render) {
        game->current_handler->render(game);
    }
}

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

void game_set_state(game_t* game, game_state_t new_state) {
    if (!game) return;
    game->next_state = new_state;
}

void game_change_level(game_t* game, int level) {
    if (!game || level < 1 || level > get_max_levels()) return;

    game->level = level;
    game->level_config = get_level_config(level);

    // Reset game elements
    score_reset(game);

    // Destroy existing snake and food
    if (game->snake) {
        snake_destroy(game->snake);
        game->snake = NULL;
    }

    if (game->food) {
        food_destroy(game->food);
        game->food = NULL;
    }

    // Recalculate board size in case terminal was resized
    game_calculate_board_size(game);

    // Create new snake at center of board
    int start_x = game->board_offset_x + game->board_width / 2;
    int start_y = game->board_offset_y + game->board_height / 2;
    game->snake = snake_create(start_x, start_y, DIR_RIGHT);

    // Create and spawn food
    game->food = food_create();
    if (game->food) {
        food_spawn(game->food, game);
    }
}

level_config_t* get_level_config(int level) {
    if (level < 1 || level > get_max_levels()) {
        return &level_configs[0]; // Return first level as default
    }

    return &level_configs[level - 1];
}

int get_max_levels(void) {
    return sizeof(level_configs) / sizeof(level_configs[0]);
}

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

bool game_is_point_in_bounds(game_t* game, point_t p) {
    if (!game) return false;

    return (p.x >= game->board_offset_x &&
            p.x < game->board_offset_x + game->board_width &&
            p.y >= game->board_offset_y &&
            p.y < game->board_offset_y + game->board_height);
}

bool game_is_point_on_border(game_t* game, point_t p) {
    if (!game) return false;

    return (p.x == game->board_offset_x ||
            p.x == game->board_offset_x + game->board_width - 1 ||
            p.y == game->board_offset_y ||
            p.y == game->board_offset_y + game->board_height - 1);
}