#include "snake.h"
#include <stdlib.h>
#include <string.h>

// Static behavior instance
static snake_behavior_t normal_behavior = {
    .move_snake = snake_move_normal,
    .check_collision = snake_check_collision_normal,
    .grow = snake_grow_normal
};

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

void snake_grow_normal(snake_t* snake) {
    if (!snake) return;
    snake->should_grow = true;
}

void snake_set_direction(snake_t* snake, direction_t new_dir) {
    if (!snake) return;
    snake->next_direction = new_dir;
}

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

point_t snake_get_head_position(snake_t* snake) {
    if (!snake || !snake->head) {
        return point_create(0, 0);
    }
    return snake->head->position;
}

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

snake_behavior_t* get_normal_snake_behavior(void) {
    return &normal_behavior;
}