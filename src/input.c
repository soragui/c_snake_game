#include "input.h"
#include "snake.h"
#include <ncurses.h>

void input_init(void) {
    // Enable keypad for arrow keys
    keypad(stdscr, TRUE);

    // Make getch() non-blocking
    nodelay(stdscr, TRUE);

    // Don't echo input
    noecho();

    // Hide cursor
    curs_set(0);
}

int input_get_key(void) {
    return getch();
}

bool input_is_direction_key(int key) {
    return (key == KEY_UP || key == KEY_DOWN ||
            key == KEY_LEFT || key == KEY_RIGHT ||
            key == 'w' || key == 'W' ||
            key == 's' || key == 'S' ||
            key == 'a' || key == 'A' ||
            key == 'd' || key == 'D');
}

direction_t input_key_to_direction(int key) {
    switch (key) {
        case KEY_UP:
        case 'w':
        case 'W':
            return DIR_UP;

        case KEY_DOWN:
        case 's':
        case 'S':
            return DIR_DOWN;

        case KEY_LEFT:
        case 'a':
        case 'A':
            return DIR_LEFT;

        case KEY_RIGHT:
        case 'd':
        case 'D':
            return DIR_RIGHT;

        default:
            return DIR_UP; // Default fallback
    }
}

void input_handle_start_screen(game_t* game, int key) {
    if (!game) return;

    switch (key) {
        case KEY_UP:
        case 'w':
        case 'W':
            if (game->selected_level > 1) {
                game->selected_level--;
            }
            break;

        case KEY_DOWN:
        case 's':
        case 'S':
            if (game->selected_level < get_max_levels()) {
                game->selected_level++;
            }
            break;

        case KEY_ENTER:
        case KEY_SPACE:
            game_change_level(game, game->selected_level);
            game_set_state(game, STATE_PLAYING);
            break;

        case KEY_ESC:
        case 'q':
        case 'Q':
            game_set_state(game, STATE_EXIT);
            break;
    }
}

void input_handle_game_screen(game_t* game, int key) {
    if (!game) return;

    if (input_is_direction_key(key)) {
        direction_t new_dir = input_key_to_direction(key);
        if (game->snake) {
            snake_set_direction(game->snake, new_dir);
        }
    } else {
        switch (key) {
            case 'p':
            case 'P':
            case KEY_SPACE:
                game_set_state(game, STATE_PAUSED);
                break;

            case KEY_ESC:
            case 'q':
            case 'Q':
                game_set_state(game, STATE_START_SCREEN);
                break;
        }
    }
}

void input_handle_game_over_screen(game_t* game, int key) {
    if (!game) return;

    switch (key) {
        case KEY_ENTER:
        case KEY_SPACE:
        case 'r':
        case 'R':
            // Restart game with same level
            game_change_level(game, game->level);
            game_set_state(game, STATE_PLAYING);
            break;

        case KEY_ESC:
        case 'm':
        case 'M':
            game_set_state(game, STATE_START_SCREEN);
            break;

        case 'q':
        case 'Q':
            game_set_state(game, STATE_EXIT);
            break;
    }
}