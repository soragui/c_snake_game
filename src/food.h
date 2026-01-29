#ifndef FOOD_H
#define FOOD_H

#include "game.h"
#include "utils.h"
#include <stdbool.h>

// Food structure
struct food {
    point_t position;
    food_type_t* type;
    bool active;
};

// Food creation and destruction
food_t* food_create(void);
void food_destroy(food_t* food);

// Food operations
void food_spawn(food_t* food, game_t* game);
void food_consume(food_t* food, game_t* game);
bool food_is_at_position(food_t* food, point_t position);

// Food placement
point_t food_find_valid_position(game_t* game);
bool food_is_position_valid(game_t* game, point_t position);

// Food type functions
void food_apple_on_eaten(game_t* game, food_t* food);

// Food type configurations
food_type_t* get_apple_food_type(void);
food_type_t* get_food_types_for_level(int level, int* count);

#endif // FOOD_H