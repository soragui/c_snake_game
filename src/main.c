#include "game.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************
 * @brief 程序入口函数 - 初始化并运行贪吃蛇游戏
 * 
 * 主函数流程:
 * 1. 检查终端尺寸是否满足游戏要求
 * 2. 创建并初始化游戏实例
 * 3. 运行游戏主循环
 * 4. 清理资源并退出
 * 
 * @return int 退出码 - 0 表示成功，1 表示失败
 *****************************************************************************/
int main(void) {
    // Check if terminal size is adequate before starting
    if (!is_terminal_size_valid()) {
        int width, height;
        get_terminal_size(&width, &height);
        printf("Terminal too small!\n");
        printf("Current size: %dx%d\n", width, height);
        printf("Minimum required: %dx%d\n", MIN_TERMINAL_WIDTH, MIN_TERMINAL_HEIGHT);
        printf("Please resize your terminal and try again.\n");
        return 1;
    }

    // Create and initialize game
    game_t* game = game_create();
    if (!game) {
        fprintf(stderr, "Failed to create game!\n");
        return 1;
    }

    game_init(game);
    if (!game->running) {
        game_destroy(game);
        return 1;
    }

    // Run the main game loop
    game_run(game);

    // Cleanup
    game_destroy(game);

    printf("Thanks for playing Snake!\n");
    return 0;
}
