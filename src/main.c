#include "game.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

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