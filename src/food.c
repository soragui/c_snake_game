#include "food.h"
#include "snake.h"
#include "score.h"
#include <stdlib.h>

// Static food type instances
static food_type_t apple_type = {
    .value = 10,
    .symbol = '*',
    .color_pair = COLOR_FOOD,
    .on_eaten = food_apple_on_eaten
};

food_t* food_create(void) {
    food_t* food = malloc(sizeof(food_t));
    if (!food) return NULL;

    food->position = point_create(0, 0);
    food->type = &apple_type;
    food->active = false;

    return food;
}

void food_destroy(food_t* food) {
    if (food) {
        free(food);
    }
}

void food_spawn(food_t* food, game_t* game) {
    if (!food || !game) return;

    food->position = food_find_valid_position(game);
    food->type = &apple_type; // For now, always spawn apples
    food->active = true;
}

void food_consume(food_t* food, game_t* game) {
    if (!food || !game || !food->active) return;

    // Call the food type's consumption handler
    if (food->type && food->type->on_eaten) {
        food->type->on_eaten(game, food);
    }

    food->active = false;
}

bool food_is_at_position(food_t* food, point_t position) {
    if (!food || !food->active) return false;
    return point_equals(food->position, position);
}

point_t food_find_valid_position(game_t* game) {
    if (!game) return point_create(0, 0);

    point_t position;
    int max_attempts = 100;
    int attempts = 0;

    do {
        // Generate random position within game board
        int x = get_random(game->board_offset_x + 1,
                          game->board_offset_x + game->board_width - 2);
        int y = get_random(game->board_offset_y + 1,
                          game->board_offset_y + game->board_height - 2);
        position = point_create(x, y);
        attempts++;
    } while (!food_is_position_valid(game, position) && attempts < max_attempts);

    // If we couldn't find a valid position, return center of board
    if (attempts >= max_attempts) {
        position = point_create(
            game->board_offset_x + game->board_width / 2,
            game->board_offset_y + game->board_height / 2
        );
    }

    return position;
}

bool food_is_position_valid(game_t* game, point_t position) {
    if (!game) return false;

    // Check if position is within game board boundaries (not on border)
    if (position.x <= game->board_offset_x ||
        position.x >= game->board_offset_x + game->board_width - 1 ||
        position.y <= game->board_offset_y ||
        position.y >= game->board_offset_y + game->board_height - 1) {
        return false;
    }

    // Check if position is occupied by snake
    if (game->snake && snake_contains_point(game->snake, position)) {
        return false;
    }

    return true;
}

void food_apple_on_eaten(game_t* game, food_t* food) {
    if (!game || !food) return;

    // Grow the snake
    if (game->snake && game->snake->behavior && game->snake->behavior->grow) {
        game->snake->behavior->grow(game->snake);
    }

    // Add score
    int points = score_calculate_food_points(game, food);
    score_add_points(game, points);
}

food_type_t* get_apple_food_type(void) {
    return &apple_type;
}

food_type_t* get_food_types_for_level(int level, int* count) {
    // For now, all levels just have apples
    // This can be extended later for different food types per level
    (void)level; // Suppress unused parameter warning
    if (count) *count = 1;
    return &apple_type;
}