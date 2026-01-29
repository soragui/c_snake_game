#define _POSIX_C_SOURCE 200809L
#include "utils.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>

void init_random(void) {
    srand((unsigned int)time(NULL));
}

int get_random(int min, int max) {
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    return min + rand() % (max - min + 1);
}

void get_terminal_size(int* width, int* height) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        *width = w.ws_col;
        *height = w.ws_row;
    } else {
        // Fallback to default size
        *width = 80;
        *height = 24;
    }
}

bool is_terminal_size_valid(void) {
    int width, height;
    get_terminal_size(&width, &height);
    return width >= MIN_TERMINAL_WIDTH && height >= MIN_TERMINAL_HEIGHT;
}

void sleep_ms(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

point_t point_create(int x, int y) {
    point_t p = {x, y};
    return p;
}

bool point_equals(point_t a, point_t b) {
    return a.x == b.x && a.y == b.y;
}

point_t point_add(point_t a, point_t b) {
    point_t result = {a.x + b.x, a.y + b.y};
    return result;
}

point_t direction_to_point(direction_t dir) {
    switch (dir) {
        case DIR_UP:    return point_create(0, -1);
        case DIR_DOWN:  return point_create(0, 1);
        case DIR_LEFT:  return point_create(-1, 0);
        case DIR_RIGHT: return point_create(1, 0);
        default:        return point_create(0, 0);
    }
}

direction_t opposite_direction(direction_t dir) {
    switch (dir) {
        case DIR_UP:    return DIR_DOWN;
        case DIR_DOWN:  return DIR_UP;
        case DIR_LEFT:  return DIR_RIGHT;
        case DIR_RIGHT: return DIR_LEFT;
        default:        return dir;
    }
}