#include "snake.h"
#include <stdlib.h>
#include <string.h>

// Static behavior instance
static snake_behavior_t normal_behavior = {
    .move_snake = snake_move_normal,
    .check_collision = snake_check_collision_normal,
    .grow = snake_grow_normal
};

/******************************************************************************
 * @brief 创建蛇实例
 * 
 * 在指定位置创建蛇，初始长度为 1，方向为指定方向
 * 
 * @param start_x 起始 X 坐标
 * @param start_y 起始 Y 坐标
 * @param initial_dir 初始方向
 * @return snake_t* 蛇实例指针，失败返回 NULL
 *****************************************************************************/
snake_t* snake_create(int start_x, int start_y, direction_t initial_dir) {
    snake_t* snake = malloc(sizeof(snake_t));
    if (!snake) return NULL;

    // Create initial head segment
    snake_segment_t* head = malloc(sizeof(snake_segment_t));
    if (!head) {
        free(snake);
        return NULL;
    }

    head->position = point_create(start_x, start_y);
    head->next = NULL;

    snake->head = head;
    snake->tail = head;
    snake->direction = initial_dir;
    snake->next_direction = initial_dir;
    snake->length = 1;
    snake->behavior = &normal_behavior;
    snake->should_grow = false;

    return snake;
}

/******************************************************************************
 * @brief 销毁蛇实例并释放所有蛇身段
 * 
 * 遍历链表释放所有蛇身段，最后释放蛇结构体本身
 * 
 * @param snake 蛇实例指针
 *****************************************************************************/
void snake_destroy(snake_t* snake) {
    if (!snake) return;

    snake_segment_t* current = snake->head;
    while (current) {
        snake_segment_t* next = current->next;
        free(current);
        current = next;
    }

    free(snake);
}

/******************************************************************************
 * @brief 蛇的正常移动行为
 * 
 * 根据当前方向移动蛇：
 * 1. 更新方向（防止立即反向）
 * 2. 计算新头部位置
 * 3. 创建新头部段
 * 4. 如果不生长则移除尾部
 * 
 * @param snake 蛇实例指针
 * @param game 游戏实例指针
 *****************************************************************************/
void snake_move_normal(snake_t* snake, game_t* game) {
    if (!snake || !game) return;

    // Update direction (prevent immediate reversal)
    if (snake->next_direction != opposite_direction(snake->direction)) {
        snake->direction = snake->next_direction;
    }

    // Calculate new head position
    point_t movement = direction_to_point(snake->direction);
    point_t new_head_pos = point_add(snake->head->position, movement);

    // Create new head segment
    snake_segment_t* new_head = malloc(sizeof(snake_segment_t));
    if (!new_head) return; // Memory allocation failed

    new_head->position = new_head_pos;
    new_head->next = snake->head;
    snake->head = new_head;
    snake->length++;

    // Remove tail unless growing
    if (!snake->should_grow) {
        snake_remove_tail(snake);
    } else {
        snake->should_grow = false;
    }
}

/******************************************************************************
 * @brief 蛇的正常碰撞检测行为
 * 
 * 检测两种碰撞：
 * 1. 墙壁碰撞 - 检查头部是否超出边界或在边框上
 * 2. 自身碰撞 - 检查头部是否与身体任何部分重叠
 * 
 * @param snake 蛇实例指针
 * @param game 游戏实例指针
 * @return bool 发生碰撞返回 true，否则返回 false
 *****************************************************************************/
bool snake_check_collision_normal(snake_t* snake, game_t* game) {
    if (!snake || !game) return true;

    point_t head_pos = snake_get_head_position(snake);

    // Check wall collision
    if (!game_is_point_in_bounds(game, head_pos) ||
        game_is_point_on_border(game, head_pos)) {
        return true;
    }

    // Check self collision (skip head when checking)
    snake_segment_t* current = snake->head->next;
    while (current) {
        if (point_equals(head_pos, current->position)) {
            return true;
        }
        current = current->next;
    }

    return false;
}

/******************************************************************************
 * @brief 蛇的正常生长行为
 * 
 * 标记蛇应该在下次移动时生长（不移除尾部）
 * 
 * @param snake 蛇实例指针
 *****************************************************************************/
void snake_grow_normal(snake_t* snake) {
    if (!snake) return;
    snake->should_grow = true;
}

/******************************************************************************
 * @brief 设置蛇的移动方向
 * 
 * 设置下一个方向，实际方向更新在移动时进行（防止立即反向）
 * 
 * @param snake 蛇实例指针
 * @param new_dir 新方向
 *****************************************************************************/
void snake_set_direction(snake_t* snake, direction_t new_dir) {
    if (!snake) return;
    snake->next_direction = new_dir;
}

/******************************************************************************
 * @brief 添加蛇身段
 * 
 * 在蛇尾部添加一个新的身体段
 * 
 * @param snake 蛇实例指针
 * @param position 新段的位置
 *****************************************************************************/
void snake_add_segment(snake_t* snake, point_t position) {
    if (!snake) return;

    snake_segment_t* new_segment = malloc(sizeof(snake_segment_t));
    if (!new_segment) return;

    new_segment->position = position;
    new_segment->next = NULL;

    if (snake->tail) {
        snake->tail->next = new_segment;
    }
    snake->tail = new_segment;

    if (!snake->head) {
        snake->head = new_segment;
    }

    snake->length++;
}

/******************************************************************************
 * @brief 移除蛇尾部
 * 
 * 移除蛇的最后一个身体段，用于正常移动时保持长度不变
 * 
 * @param snake 蛇实例指针
 *****************************************************************************/
void snake_remove_tail(snake_t* snake) {
    if (!snake || !snake->head || snake->length <= 1) return;

    if (snake->length == 1) {
        free(snake->head);
        snake->head = NULL;
        snake->tail = NULL;
        snake->length = 0;
        return;
    }

    // Find second-to-last segment
    snake_segment_t* current = snake->head;
    while (current->next && current->next != snake->tail) {
        current = current->next;
    }

    // Remove tail
    free(snake->tail);
    snake->tail = current;
    current->next = NULL;
    snake->length--;
}

/******************************************************************************
 * @brief 重置蛇的位置
 * 
 * 清除所有身体段，将蛇重置为单个头部段在指定位置
 * 
 * @param snake 蛇实例指针
 * @param x 新 X 坐标
 * @param y 新 Y 坐标
 * @param dir 新方向
 *****************************************************************************/
void snake_reset_position(snake_t* snake, int x, int y, direction_t dir) {
    if (!snake) return;

    // Clear existing segments except head
    snake_segment_t* current = snake->head->next;
    while (current) {
        snake_segment_t* next = current->next;
        free(current);
        current = next;
    }

    // Reset head
    snake->head->position = point_create(x, y);
    snake->head->next = NULL;
    snake->tail = snake->head;
    snake->direction = dir;
    snake->next_direction = dir;
    snake->length = 1;
    snake->should_grow = false;
}

/******************************************************************************
 * @brief 获取蛇头部位置
 * 
 * @param snake 蛇实例指针
 * @return point_t 头部位置坐标
 *****************************************************************************/
point_t snake_get_head_position(snake_t* snake) {
    if (!snake || !snake->head) {
        return point_create(0, 0);
    }
    return snake->head->position;
}

/******************************************************************************
 * @brief 检查蛇是否包含指定点
 * 
 * 遍历蛇的所有身体段，检查是否有任何段在指定位置
 * 
 * @param snake 蛇实例指针
 * @param point 要检查的点
 * @return bool 包含该点返回 true，否则返回 false
 *****************************************************************************/
bool snake_contains_point(snake_t* snake, point_t point) {
    if (!snake) return false;

    snake_segment_t* current = snake->head;
    while (current) {
        if (point_equals(current->position, point)) {
            return true;
        }
        current = current->next;
    }

    return false;
}

/******************************************************************************
 * @brief 检查蛇头是否与身体碰撞
 * 
 * 检查蛇头部位置是否与任何身体段重叠
 * 
 * @param snake 蛇实例指针
 * @return bool 发生碰撞返回 true，否则返回 false
 *****************************************************************************/
bool snake_head_collides_with_body(snake_t* snake) {
    if (!snake || !snake->head || !snake->head->next) {
        return false;
    }

    point_t head_pos = snake->head->position;
    snake_segment_t* current = snake->head->next;

    while (current) {
        if (point_equals(head_pos, current->position)) {
            return true;
        }
        current = current->next;
    }

    return false;
}

/******************************************************************************
 * @brief 获取正常蛇行为配置
 * 
 * @return snake_behavior_t* 正常行为配置指针
 *****************************************************************************/
snake_behavior_t* get_normal_snake_behavior(void) {
    return &normal_behavior;
}
