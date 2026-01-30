#include "score.h"
#include "utils.h"
#include "food.h"
#include <stdio.h>
#include <stdlib.h>

void score_init(game_t* game) {
    if (!game) return;

    game->score = 0;
    game->high_score = score_load_high_score();
}

void score_add_points(game_t* game, int points) {
    if (!game || points <= 0) return;

    game->score += points;

    // Update high score if necessary
    if (game->score > game->high_score) {
        game->high_score = game->score;
    }
}

void score_reset(game_t* game) {
    if (!game) return;
    game->score = 0;
}

int score_load_high_score(void) {
    FILE* file = fopen(HIGHSCORE_FILE, "r");
    if (!file) {
        return 0; // No high score file exists yet
    }

    int high_score = 0;
    if (fscanf(file, "%d", &high_score) != 1) {
        high_score = 0; // Failed to read score
    }

    fclose(file);
    return high_score;
}

void score_save_high_score(int score) {
    // create data dir on the first time
    FILE* file = fopen(HIGHSCORE_FILE, "w");
    if (!file) {
        return; // Failed to open file for writing
    }

    fprintf(file, "%d\n", score);
    fclose(file);
}

bool score_is_new_high_score(int score) {
    return score > score_load_high_score();
}

int score_calculate_food_points(game_t* game, food_t* food) {
    if (!game || !food || !food->type) {
        return 0;
    }

    int base_points = food->type->value;
    int multiplier = score_get_level_multiplier(game->level);

    return base_points * multiplier;
}

int score_get_level_multiplier(int level) {
    // Higher levels give more points
    switch (level) {
        case 1: return 1;  // Easy
        case 2: return 2;  // Medium
        case 3: return 3;  // Hard
        case 4: return 4;  // Very Hard
        case 5: return 5;  // Extreme
        default: return 1;
    }
}