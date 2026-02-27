#define _POSIX_C_SOURCE 200809L
#include "utils.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>

/******************************************************************************
 * @brief 初始化随机数生成器
 * 
 * 使用当前时间作为种子初始化随机数生成器
 *****************************************************************************/
void init_random(void) {
    srand((unsigned int)time(NULL));
}

/******************************************************************************
 * @brief 生成指定范围内的随机数
 * 
 * 生成 [min, max] 范围内的随机整数（包含边界）
 * 
 * @param min 最小值
 * @param max 最大值
 * @return int 随机数
 *****************************************************************************/
int get_random(int min, int max) {
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    return min + rand() % (max - min + 1);
}

/******************************************************************************
 * @brief 获取终端尺寸
 * 
 * 使用 ioctl 获取终端行列数，失败则返回默认值 80x24
 * 
 * @param width 输出参数 - 终端宽度（列数）
 * @param height 输出参数 - 终端高度（行数）
 *****************************************************************************/
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

/******************************************************************************
 * @brief 检查终端尺寸是否有效
 * 
 * 终端尺寸必须大于等于 MIN_TERMINAL_WIDTH x MIN_TERMINAL_HEIGHT
 * 
 * @return bool 尺寸有效返回 true，否则返回 false
 *****************************************************************************/
bool is_terminal_size_valid(void) {
    int width, height;
    get_terminal_size(&width, &height);
    return width >= MIN_TERMINAL_WIDTH && height >= MIN_TERMINAL_HEIGHT;
}

/******************************************************************************
 * @brief 休眠指定毫秒数
 * 
 * 使用 nanosleep 实现精确的毫秒级休眠
 * 
 * @param milliseconds 休眠毫秒数
 *****************************************************************************/
void sleep_ms(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

/******************************************************************************
 * @brief 创建点
 * 
 * @param x X 坐标
 * @param y Y 坐标
 * @return point_t 创建的点
 *****************************************************************************/
point_t point_create(int x, int y) {
    point_t p = {x, y};
    return p;
}

/******************************************************************************
 * @brief 比较两个点是否相等
 * 
 * @param a 第一个点
 * @param b 第二个点
 * @return bool 两点相等返回 true，否则返回 false
 *****************************************************************************/
bool point_equals(point_t a, point_t b) {
    return a.x == b.x && a.y == b.y;
}

/******************************************************************************
 * @brief 点加法
 * 
 * 计算两个点的向量和
 * 
 * @param a 第一个点
 * @param b 第二个点
 * @return point_t 两点之和
 *****************************************************************************/
point_t point_add(point_t a, point_t b) {
    point_t result = {a.x + b.x, a.y + b.y};
    return result;
}

/******************************************************************************
 * @brief 将方向转换为位移向量
 * 
 * 方向映射：
 * - DIR_UP    -> (0, -1)
 * - DIR_DOWN  -> (0, 1)
 * - DIR_LEFT  -> (-1, 0)
 * - DIR_RIGHT -> (1, 0)
 * 
 * @param dir 方向
 * @return point_t 对应的位移向量
 *****************************************************************************/
point_t direction_to_point(direction_t dir) {
    switch (dir) {
        case DIR_UP:    return point_create(0, -1);
        case DIR_DOWN:  return point_create(0, 1);
        case DIR_LEFT:  return point_create(-1, 0);
        case DIR_RIGHT: return point_create(1, 0);
        default:        return point_create(0, 0);
    }
}

/******************************************************************************
 * @brief 获取相反方向
 * 
 * @param dir 方向
 * @return direction_t 相反方向
 *****************************************************************************/
direction_t opposite_direction(direction_t dir) {
    switch (dir) {
        case DIR_UP:    return DIR_DOWN;
        case DIR_DOWN:  return DIR_UP;
        case DIR_LEFT:  return DIR_RIGHT;
        case DIR_RIGHT: return DIR_LEFT;
        default:        return dir;
    }
}
