#ifndef UI_H
#define UI_H

#include "game.h"
#include <ncurses.h>

// UI initialization and cleanup
void ui_init(void);
void ui_cleanup(void);
void ui_setup_colors(void);

// Screen management
void ui_clear_screen(void);
void ui_refresh_screen(void);

// Drawing primitives
void ui_draw_border(int width, int height, int offset_x, int offset_y);
void ui_draw_text(int x, int y, const char* text, int color_pair);
void ui_draw_text_centered(int y, const char* text, int color_pair);
void ui_draw_char(int x, int y, char ch, int color_pair);

// Game element rendering
void ui_draw_snake(snake_t* snake);
void ui_draw_food(food_t* food);
void ui_draw_score(game_t* game);

// Screen-specific rendering
void ui_render_start_screen(game_t* game);
void ui_render_game_screen(game_t* game);
void ui_render_game_over_screen(game_t* game);

// Menu utilities
void ui_draw_menu(const char** items, int item_count, int selected, int start_y);
void ui_draw_level_selector(int selected_level);

// State handlers for UI
state_handler_t* get_start_screen_handler(void);
state_handler_t* get_game_screen_handler(void);
state_handler_t* get_game_over_handler(void);

// Renderer implementation
renderer_t* get_ncurses_renderer(void);

#endif // UI_H