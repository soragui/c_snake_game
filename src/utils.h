#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// Basic data types
typedef struct {
    int x;
    int y;
} point_t;

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} direction_t;

// Color definitions
#define COLOR_SNAKE     1
#define COLOR_FOOD      2
#define COLOR_WALL      3
#define COLOR_UI        4
#define COLOR_HIGHLIGHT 5

// Game constants
#define MIN_TERMINAL_WIDTH  40
#define MIN_TERMINAL_HEIGHT 20
#define MAX_LEVELS          5
#define HIGHSCORE_FILE      "data/highscore.txt"

// Utility functions
void init_random(void);
int get_random(int min, int max);
void get_terminal_size(int* width, int* height);
bool is_terminal_size_valid(void);
void sleep_ms(int milliseconds);

// Point utilities
point_t point_create(int x, int y);
bool point_equals(point_t a, point_t b);
point_t point_add(point_t a, point_t b);

// Direction utilities
point_t direction_to_point(direction_t dir);
direction_t opposite_direction(direction_t dir);

#endif // UTILS_H