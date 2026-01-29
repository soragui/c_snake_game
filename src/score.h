#ifndef SCORE_H
#define SCORE_H

#include "game.h"

// Score operations
void score_init(game_t* game);
void score_add_points(game_t* game, int points);
void score_reset(game_t* game);

// High score management
int score_load_high_score(void);
void score_save_high_score(int score);
bool score_is_new_high_score(int score);

// Score calculation
int score_calculate_food_points(game_t* game, food_t* food);
int score_get_level_multiplier(int level);

#endif // SCORE_H