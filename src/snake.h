#ifndef SNAKE_H
#define SNAKE_H

#include "game.h"
#include "utils.h"
#include <stdbool.h>

// Snake segment structure
typedef struct snake_segment {
    point_t position;
    struct snake_segment* next;
} snake_segment_t;

// Snake structure
struct snake {
    snake_segment_t* head;
    snake_segment_t* tail;
    direction_t direction;
    direction_t next_direction;
    int length;
    snake_behavior_t* behavior;
    bool should_grow;
};

// Snake creation and destruction
snake_t* snake_create(int start_x, int start_y, direction_t initial_dir);
void snake_destroy(snake_t* snake);

// Snake behavior functions
void snake_move_normal(snake_t* snake, game_t* game);
bool snake_check_collision_normal(snake_t* snake, game_t* game);
void snake_grow_normal(snake_t* snake);

// Snake operations
void snake_set_direction(snake_t* snake, direction_t new_dir);
void snake_add_segment(snake_t* snake, point_t position);
void snake_remove_tail(snake_t* snake);
void snake_reset_position(snake_t* snake, int x, int y, direction_t dir);

// Snake queries
point_t snake_get_head_position(snake_t* snake);
bool snake_contains_point(snake_t* snake, point_t point);
bool snake_head_collides_with_body(snake_t* snake);

// Behavior configurations
snake_behavior_t* get_normal_snake_behavior(void);

#endif // SNAKE_H