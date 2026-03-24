#include "obstacle.h"
#include "ui.h"
#include <stdlib.h>
#include <string.h>

// Static maze instances for different levels
static maze_t g_mazes[5];  // One maze per level (levels 1-5, wraparound levels don't have mazes)
static int mazes_initialized = 0;

/******************************************************************************
 * @brief 初始化迷宫
 *
 * 为指定等级创建迷宫布局
 * 等级 1-2：无障碍
 * 等级 3：简单迷宫（4 个障碍物）
 * 等级 4：中等迷宫（8 个障碍物）
 * 等级 5：复杂迷宫（12 个障碍物）
 *
 * @param maze 迷宫结构体指针
 * @param level 游戏等级
 *****************************************************************************/
void maze_init(maze_t* maze, int level) {
    if (!maze) return;

    maze->num_obstacles = 0;
    maze->level = level;

    // Levels 1-2: no obstacles (classic snake)
    if (level <= 2) {
        return;
    }

    // Level 3: Simple maze - 4 obstacles forming a small center square
    if (level == 3) {
        // Will be positioned relative to board size at render time
        // These are relative offsets that will be scaled
        maze->obstacles[0] = (obstacle_t){.start = {-1, -1}, .end = {1, -1}, .is_horizontal = true};
        maze->obstacles[1] = (obstacle_t){.start = {-1, 1}, .end = {1, 1}, .is_horizontal = true};
        maze->obstacles[2] = (obstacle_t){.start = {-1, -1}, .end = {-1, 1}, .is_horizontal = false};
        maze->obstacles[3] = (obstacle_t){.start = {1, -1}, .end = {1, 1}, .is_horizontal = false};
        maze->num_obstacles = 4;
    }

    // Level 4: Medium maze - 8 obstacles
    if (level == 4) {
        // Center square
        maze->obstacles[0] = (obstacle_t){.start = {-2, -2}, .end = {2, -2}, .is_horizontal = true};
        maze->obstacles[1] = (obstacle_t){.start = {-2, 2}, .end = {2, 2}, .is_horizontal = true};
        maze->obstacles[2] = (obstacle_t){.start = {-2, -2}, .end = {-2, 2}, .is_horizontal = false};
        maze->obstacles[3] = (obstacle_t){.start = {2, -2}, .end = {2, 2}, .is_horizontal = false};

        // Outer corners
        maze->obstacles[4] = (obstacle_t){.start = {-4, -4}, .end = {-2, -4}, .is_horizontal = true};
        maze->obstacles[5] = (obstacle_t){.start = {2, -4}, .end = {4, -4}, .is_horizontal = true};
        maze->obstacles[6] = (obstacle_t){.start = {-4, 4}, .end = {-2, 4}, .is_horizontal = true};
        maze->obstacles[7] = (obstacle_t){.start = {2, 4}, .end = {4, 4}, .is_horizontal = true};
        maze->num_obstacles = 8;
    }

    // Level 5: Complex maze - 12 obstacles
    if (level == 5) {
        // Inner square
        maze->obstacles[0] = (obstacle_t){.start = {-2, -2}, .end = {2, -2}, .is_horizontal = true};
        maze->obstacles[1] = (obstacle_t){.start = {-2, 2}, .end = {2, 2}, .is_horizontal = true};
        maze->obstacles[2] = (obstacle_t){.start = {-2, -2}, .end = {-2, 2}, .is_horizontal = false};
        maze->obstacles[3] = (obstacle_t){.start = {2, -2}, .end = {2, 2}, .is_horizontal = false};

        // Middle ring
        maze->obstacles[4] = (obstacle_t){.start = {-4, -4}, .end = {4, -4}, .is_horizontal = true};
        maze->obstacles[5] = (obstacle_t){.start = {-4, 4}, .end = {4, 4}, .is_horizontal = true};
        maze->obstacles[6] = (obstacle_t){.start = {-4, -4}, .end = {-4, 4}, .is_horizontal = false};
        maze->obstacles[7] = (obstacle_t){.start = {4, -4}, .end = {4, 4}, .is_horizontal = false};

        // Outer barriers
        maze->obstacles[8] = (obstacle_t){.start = {-6, 0}, .end = {-4, 0}, .is_horizontal = true};
        maze->obstacles[9] = (obstacle_t){.start = {4, 0}, .end = {6, 0}, .is_horizontal = true};
        maze->obstacles[10] = (obstacle_t){.start = {0, -6}, .end = {0, -4}, .is_horizontal = false};
        maze->obstacles[11] = (obstacle_t){.start = {0, 4}, .end = {0, 6}, .is_horizontal = false};
        maze->num_obstacles = 12;
    }
}

/******************************************************************************
 * @brief 销毁迷宫
 *
 * @param maze 迷宫结构体指针
 *****************************************************************************/
void maze_destroy(maze_t* maze) {
    if (!maze) return;
    maze->num_obstacles = 0;
}

/******************************************************************************
 * @brief 检查位置是否与障碍物碰撞
 *
 * @param maze 迷宫结构体指针
 * @param pos 要检查的位置
 * @return bool 发生碰撞返回 true，否则返回 false
 *****************************************************************************/
bool obstacle_check_collision(maze_t* maze, point_t pos) {
    if (!maze || maze->num_obstacles == 0) return false;

    for (int i = 0; i < maze->num_obstacles; i++) {
        obstacle_t* obs = &maze->obstacles[i];

        // Simple point collision check
        // For horizontal obstacle: check if y matches and x is between start and end
        // For vertical obstacle: check if x matches and y is between start and end
        if (obs->is_horizontal) {
            if (pos.y == obs->start.y) {
                int min_x = obs->start.x < obs->end.x ? obs->start.x : obs->end.x;
                int max_x = obs->start.x > obs->end.x ? obs->start.x : obs->end.x;
                if (pos.x >= min_x && pos.x <= max_x) {
                    return true;
                }
            }
        } else {
            if (pos.x == obs->start.x) {
                int min_y = obs->start.y < obs->end.y ? obs->start.y : obs->end.y;
                int max_y = obs->start.y > obs->end.y ? obs->start.y : obs->end.y;
                if (pos.y >= min_y && pos.y <= max_y) {
                    return true;
                }
            }
        }
    }

    return false;
}

/******************************************************************************
 * @brief 渲染所有障碍物
 *
 * 使用 '#' 字符绘制障碍物
 * 将相对坐标转换为绝对屏幕坐标
 *
 * @param maze 迷宫结构体指针
 * @param board_offset_x 游戏板 X 偏移
 * @param board_offset_y 游戏板 Y 偏移
 *****************************************************************************/
void obstacle_render(maze_t* maze, int board_offset_x, int board_offset_y) {
    if (!maze || maze->num_obstacles == 0) return;

    for (int i = 0; i < maze->num_obstacles; i++) {
        obstacle_t* obs = &maze->obstacles[i];

        if (obs->is_horizontal) {
            int min_x = obs->start.x < obs->end.x ? obs->start.x : obs->end.x;
            int max_x = obs->start.x > obs->end.x ? obs->start.x : obs->end.x;
            for (int x = min_x; x <= max_x; x++) {
                ui_draw_char(board_offset_x + x, board_offset_y + obs->start.y, '#', COLOR_WALL);
            }
        } else {
            int min_y = obs->start.y < obs->end.y ? obs->start.y : obs->end.y;
            int max_y = obs->start.y > obs->end.y ? obs->start.y : obs->end.y;
            for (int y = min_y; y <= max_y; y++) {
                ui_draw_char(board_offset_x + obs->start.x, board_offset_y + y, '#', COLOR_WALL);
            }
        }
    }
}

/******************************************************************************
 * @brief 获取指定等级的迷宫
 *
 * @param level 游戏等级
 * @return maze_t* 迷宫实例指针
 *****************************************************************************/
maze_t* get_maze_for_level(int level) {
    if (level < 1 || level > 5) return NULL;

    // Initialize mazes on first call
    if (!mazes_initialized) {
        for (int i = 0; i < 5; i++) {
            maze_init(&g_mazes[i], i + 1);
        }
        mazes_initialized = 1;
    }

    return &g_mazes[level - 1];
}

/******************************************************************************
 * @brief 检查等级是否有障碍物
 *
 * 等级 1-2 没有障碍物，等级 3-5 有
 *
 * @param level 游戏等级
 * @return bool 有障碍物返回 true，否则返回 false
 *****************************************************************************/
bool maze_has_obstacles(int level) {
    return level >= 3 && level <= 5;
}
