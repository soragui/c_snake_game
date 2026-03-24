#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "utils.h"
#include "game.h"
#include <stdbool.h>

// Maximum number of obstacles per maze
#define MAX_OBSTACLES 50
#define MAX_OBSTACLE_POINTS 20

// Obstacle structure (a line segment of barrier blocks)
typedef struct {
    point_t start;
    point_t end;
    bool is_horizontal;
} obstacle_t;

// Maze structure (collection of obstacles)
typedef struct {
    obstacle_t obstacles[MAX_OBSTACLES];
    int num_obstacles;
    int level;  // Which game level this maze is for
} maze_t;

// Maze initialization
void maze_init(maze_t* maze, int level);
void maze_destroy(maze_t* maze);

// Obstacle operations
bool obstacle_check_collision(maze_t* maze, point_t pos);
void obstacle_render(maze_t* maze, int board_offset_x, int board_offset_y);

// Maze configurations
maze_t* get_maze_for_level(int level);
bool maze_has_obstacles(int level);

#endif // OBSTACLE_H
