#include "score.h"
#include "utils.h"
#include "food.h"
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************
 * @brief 初始化分数系统
 * 
 * 重置当前分数为 0，从文件加载历史最高分
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void score_init(game_t* game) {
    if (!game) return;

    game->score = 0;
    game->high_score = score_load_high_score();
}

/******************************************************************************
 * @brief 添加分数
 * 
 * 增加指定分数，如果超过历史最高分则更新
 * 
 * @param game 游戏实例指针
 * @param points 要添加的分数
 *****************************************************************************/
void score_add_points(game_t* game, int points) {
    if (!game || points <= 0) return;

    game->score += points;

    // Update high score if necessary
    if (game->score > game->high_score) {
        game->high_score = game->score;
    }
}

/******************************************************************************
 * @brief 重置分数
 * 
 * 将当前分数重置为 0（用于开始新游戏）
 * 
 * @param game 游戏实例指针
 *****************************************************************************/
void score_reset(game_t* game) {
    if (!game) return;
    game->score = 0;
}

/******************************************************************************
 * @brief 从文件加载历史最高分
 * 
 * 从 HIGHSCORE_FILE 文件读取保存的最高分
 * 
 * @return int 历史最高分，文件不存在或读取失败返回 0
 *****************************************************************************/
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

/******************************************************************************
 * @brief 保存历史最高分到文件
 * 
 * 将最高分写入 HIGHSCORE_FILE 文件
 * 
 * @param score 要保存的分数
 *****************************************************************************/
void score_save_high_score(int score) {
    // create data dir on the first time
    FILE* file = fopen(HIGHSCORE_FILE, "w");
    if (!file) {
        return; // Failed to open file for writing
    }

    fprintf(file, "%d\n", score);
    fclose(file);
}

/******************************************************************************
 * @brief 检查是否为新的历史最高分
 * 
 * @param score 要检查的分数
 * @return bool 是新最高分返回 true，否则返回 false
 *****************************************************************************/
bool score_is_new_high_score(int score) {
    return score > score_load_high_score();
}

/******************************************************************************
 * @brief 计算吃食物获得的分数
 * 
 * 分数计算公式：基础分 × 等级倍率
 * 
 * @param game 游戏实例指针
 * @param food 食物实例指针
 * @return int 获得的分数
 *****************************************************************************/
int score_calculate_food_points(game_t* game, food_t* food) {
    if (!game || !food || !food->type) {
        return 0;
    }

    int base_points = food->type->value;
    int multiplier = score_get_level_multiplier(game->level);

    return base_points * multiplier;
}

/******************************************************************************
 * @brief 获取等级分数倍率
 * 
 * 等级越高倍率越高：
 * - Level 1: 1x
 * - Level 2: 2x
 * - Level 3: 3x
 * - Level 4: 4x
 * - Level 5: 5x
 * 
 * @param level 难度等级
 * @return int 分数倍率
 *****************************************************************************/
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
