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

static food_type_t bonus_type = {
    .value = 25,
    .symbol = '$',
    .color_pair = COLOR_HIGHLIGHT,  // Yellow for bonus
    .on_eaten = food_bonus_on_eaten
};

static food_type_t poison_type = {
    .value = -15,
    .symbol = 'X',
    .color_pair = COLOR_WALL,  // White/gray for poison
    .on_eaten = food_poison_on_eaten
};

static food_type_t speed_type = {
    .value = 15,
    .symbol = '!',
    .color_pair = COLOR_UI,  // Cyan for speed
    .on_eaten = food_speed_on_eaten
};

/******************************************************************************
 * @brief 创建食物实例
 * 
 * 分配并初始化食物结构体，初始状态为非激活
 * 
 * @return food_t* 食物实例指针，失败返回 NULL
 *****************************************************************************/
food_t* food_create(void) {
    food_t* food = malloc(sizeof(food_t));
    if (!food) return NULL;

    food->position = point_create(0, 0);
    food->type = &apple_type;
    food->active = false;

    return food;
}

/******************************************************************************
 * @brief 销毁食物实例
 * 
 * @param food 食物实例指针
 *****************************************************************************/
void food_destroy(food_t* food) {
    if (food) {
        free(food);
    }
}

/******************************************************************************
 * @brief 在有效位置生成食物
 *
 * 在游戏区域内找到一个有效位置（不在蛇身上）并激活食物
 * 随机选择食物类型：苹果 (70%)、奖励 (10%)、毒药 (10%)、速度 (10%)
 *
 * @param food 食物实例指针
 * @param game 游戏实例指针
 *****************************************************************************/
void food_spawn(food_t* food, game_t* game) {
    if (!food || !game) return;

    food->position = food_find_valid_position(game);

    // Random food type: 70% apple, 10% each for bonus/poison/speed
    int roll = get_random(0, 99);
    if (roll < 70) {
        food->type = &apple_type;
    } else if (roll < 80) {
        food->type = &bonus_type;
    } else if (roll < 90) {
        food->type = &poison_type;
    } else {
        food->type = &speed_type;
    }

    food->active = true;
}

/******************************************************************************
 * @brief 消耗食物
 * 
 * 调用食物类型的消耗处理器，然后标记为非激活
 * 
 * @param food 食物实例指针
 * @param game 游戏实例指针
 *****************************************************************************/
void food_consume(food_t* food, game_t* game) {
    if (!food || !game || !food->active) return;

    // Call the food type's consumption handler
    if (food->type && food->type->on_eaten) {
        food->type->on_eaten(game, food);
    }

    food->active = false;
}

/******************************************************************************
 * @brief 检查食物是否在指定位置
 * 
 * @param food 食物实例指针
 * @param position 要检查的位置
 * @return bool 食物在该位置返回 true，否则返回 false
 *****************************************************************************/
bool food_is_at_position(food_t* food, point_t position) {
    if (!food || !food->active) return false;
    return point_equals(food->position, position);
}

/******************************************************************************
 * @brief 查找有效的食物生成位置
 * 
 * 在游戏区域内随机查找一个有效位置（不在蛇身上，不在边框上）
 * 最多尝试 100 次，失败则返回中心位置
 * 
 * @param game 游戏实例指针
 * @return point_t 有效位置坐标
 *****************************************************************************/
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

/******************************************************************************
 * @brief 检查位置是否适合生成食物
 * 
 * 检查条件：
 * 1. 位置在游戏边界内（不在边框上）
 * 2. 位置不被蛇占据
 * 
 * @param game 游戏实例指针
 * @param position 要检查的位置
 * @return bool 位置有效返回 true，否则返回 false
 *****************************************************************************/
bool food_is_position_valid(game_t* game, point_t position) {
    if (!game) return false;

    // Check if position is within game board boundaries (not on border)
    if (position.x <= game->board_offset_x ||
        position.x >= game->board_offset_x + game->board_width - 1 ||
        position.y <= game->board_offset_y ||
        position.y >= game->board_offset_y + game->board_height - 1) {
        return false;
    }

    // Check if position is occupied by player 1's snake
    if (game->snake && snake_contains_point(game->snake, position)) {
        return false;
    }

    // Check if position is occupied by player 2's snake (2-player mode)
    if (game->two_player && game->snake2 && snake_contains_point(game->snake2, position)) {
        return false;
    }

    return true;
}

/******************************************************************************
 * @brief 苹果被吃掉时的处理器
 * 
 * 执行两个操作：
 * 1. 使蛇生长
 * 2. 增加分数
 * 
 * @param game 游戏实例指针
 * @param food 食物实例指针
 *****************************************************************************/
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

/******************************************************************************
 * @brief 奖励食物被吃掉时的处理器
 *
 * 奖励食物值 25 分（基础分），不使蛇生长
 *
 * @param game 游戏实例指针
 * @param food 食物实例指针
 *****************************************************************************/
void food_bonus_on_eaten(game_t* game, food_t* food) {
    if (!game || !food) return;

    // Bonus food doesn't grow the snake, just gives extra points
    int points = score_calculate_food_points(game, food);
    score_add_points(game, points);
}

/******************************************************************************
 * @brief 毒药食物被吃掉时的处理器
 *
 * 毒药食物扣除 15 分，蛇会缩短（如果长度允许）
 *
 * @param game 游戏实例指针
 * @param food 食物实例指针
 *****************************************************************************/
void food_poison_on_eaten(game_t* game, food_t* food) {
    if (!game || !food) return;

    // Poison food removes points (negative value)
    int points = score_calculate_food_points(game, food);
    score_add_points(game, points);  // points is negative, so this subtracts

    // Shrink snake (remove tail segment) - only if length > 1
    if (game->snake && game->snake->length > 1) {
        snake_remove_tail(game->snake);
    }
}

/******************************************************************************
 * @brief 速度食物被吃掉时的处理器
 *
 * 速度食物提供中等分数，使蛇生长，并给予额外奖励分
 *
 * @param game 游戏实例指针
 * @param food 食物实例指针
 *****************************************************************************/
void food_speed_on_eaten(game_t* game, food_t* food) {
    if (!game || !food) return;

    // Speed food grows the snake and gives bonus points
    if (game->snake && game->snake->behavior && game->snake->behavior->grow) {
        game->snake->behavior->grow(game->snake);
    }

    int points = score_calculate_food_points(game, food);
    score_add_points(game, points);

    // Additional 50% bonus for speed food
    score_add_points(game, points / 2);
}

/******************************************************************************
 * @brief 获取苹果食物类型配置
 *
 * @return food_type_t* 苹果类型配置指针
 *****************************************************************************/
food_type_t* get_apple_food_type(void) {
    return &apple_type;
}

/******************************************************************************
 * @brief 获取奖励食物类型配置
 *
 * @return food_type_t* 奖励类型配置指针
 *****************************************************************************/
food_type_t* get_bonus_food_type(void) {
    return &bonus_type;
}

/******************************************************************************
 * @brief 获取毒药食物类型配置
 *
 * @return food_type_t* 毒药类型配置指针
 *****************************************************************************/
food_type_t* get_poison_food_type(void) {
    return &poison_type;
}

/******************************************************************************
 * @brief 获取速度食物类型配置
 *
 * @return food_type_t* 速度类型配置指针
 *****************************************************************************/
food_type_t* get_speed_food_type(void) {
    return &speed_type;
}

/******************************************************************************
 * @brief 获取指定等级的食物类型列表
 * 
 * 当前所有等级都只返回苹果类型，可扩展为不同等级不同食物
 * 
 * @param level 难度等级
 * @param count 输出参数，返回食物类型数量
 * @return food_type_t* 食物类型数组指针
 *****************************************************************************/
food_type_t* get_food_types_for_level(int level, int* count) {
    // For now, all levels just have apples
    // This can be extended later for different food types per level
    (void)level; // Suppress unused parameter warning
    if (count) *count = 1;
    return &apple_type;
}
