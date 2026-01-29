#ifndef INPUT_H
#define INPUT_H

#include "game.h"
#include <ncurses.h>

// Key constants
#define KEY_ESC 27
#define KEY_SPACE 32
#ifndef KEY_ENTER
#define KEY_ENTER 10
#endif

// Input initialization
void input_init(void);

// Input handling
int input_get_key(void);
bool input_is_direction_key(int key);
direction_t input_key_to_direction(int key);

// State-specific input handlers
void input_handle_start_screen(game_t* game, int key);
void input_handle_game_screen(game_t* game, int key);
void input_handle_game_over_screen(game_t* game, int key);

#endif // INPUT_H