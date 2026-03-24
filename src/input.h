#ifndef INPUT_H
#define INPUT_H

#include "game.h"
#include "utils.h"
#include <ncurses.h>

// Key constants
#define KEY_ESC 27
#define KEY_SPACE 32
#ifndef KEY_ENTER
#define KEY_ENTER 10
#endif

// Default key bindings
#define DEFAULT_KEY_UP_1 KEY_UP
#define DEFAULT_KEY_UP_2 'w'
#define DEFAULT_KEY_DOWN_1 KEY_DOWN
#define DEFAULT_KEY_DOWN_2 's'
#define DEFAULT_KEY_LEFT_1 KEY_LEFT
#define DEFAULT_KEY_LEFT_2 'a'
#define DEFAULT_KEY_RIGHT_1 KEY_RIGHT
#define DEFAULT_KEY_RIGHT_2 'd'
#define DEFAULT_KEY_PAUSE_1 'p'
#define DEFAULT_KEY_PAUSE_2 'P'
#define DEFAULT_KEY_PAUSE_3 KEY_SPACE
#define DEFAULT_KEY_SELECT KEY_ENTER
#define DEFAULT_KEY_CONFIRM KEY_SPACE
#define DEFAULT_KEY_BACK KEY_ESC
#define DEFAULT_KEY_QUIT 'q'

// Key bindings structure
typedef struct {
    int up_1;
    int up_2;
    int down_1;
    int down_2;
    int left_1;
    int left_2;
    int right_1;
    int right_2;
    int pause_1;
    int pause_2;
    int pause_3;
    int select;
    int confirm;
    int back;
    int quit;
} key_bindings_t;

// Input initialization
void input_init(void);

// Key bindings management
void input_init_default_bindings(key_bindings_t* bindings);
void input_load_bindings(key_bindings_t* bindings);
void input_save_bindings(const key_bindings_t* bindings);
key_bindings_t* input_get_bindings(void);

// Input handling
int input_get_key(void);
bool input_is_direction_key(int key);
direction_t input_key_to_direction(int key);

// Player 2 input handling (for 2-player mode)
bool input_is_direction_key_p2(int key);
direction_t input_key_to_direction_p2(int key);

// State-specific input handlers
void input_handle_start_screen(game_t* game, int key);
void input_handle_game_screen(game_t* game, int key);
void input_handle_game_over_screen(game_t* game, int key);
void input_handle_pause_screen(game_t* game, int key);

#endif // INPUT_H