# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
make deps     # Check ncurses dependency
make          # Build release version (optimized)
make debug    # Build debug version with symbols
make run      # Build and run
make clean    # Remove build artifacts
```

## Architecture Overview

### Module Structure

The game uses a modular architecture with function pointer-based interfaces for extensibility:

- **main.c** - Entry point, initializes ncurses, runs main loop
- **game.c/h** - State machine (start, playing, paused, game over, exit)
- **snake.c/h** - Snake entity with linked-list segments, direction, behavior
- **food.c/h** - Food generation, types, and consumption effects
- **ui.c/h** - ncurses rendering (renderer_t interface)
- **input.c/h** - Keyboard input handling
- **score.c/h** - Score calculation, high score persistence
- **utils.c/h** - Common types (point_t, direction_t), utilities

### Key Design Patterns

**State Machine** (`game.h:13-19`): Five game states with `state_handler_t` interface (`game.h:22-28`) containing function pointers for update, render, handle_input, enter, exit.

**Strategy Pattern** via function pointers:
- `snake_behavior_t` (`game.h:30-34`): move_snake, check_collision, grow
- `food_type_t` (`game.h:36-41`): value, symbol, on_eaten callback
- `renderer_t` (`game.h:43-50`): init, cleanup, draw_text, draw_border

**Level System** (`game.h:53-60`): Five difficulty levels with different speeds, score multipliers, and behavior configurations.

### Data Flow

```
main.c → game_create() → game_init() → game_run() [main loop]
                              ↓
              [state_handler.update] → update game state
              [state_handler.render] → draw via renderer_t
              [state_handler.handle_input] → process keyboard
```

### Core Types (`utils.h`)

- `point_t`: x, y coordinates
- `direction_t`: DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT
- Color constants: COLOR_SNAKE, COLOR_FOOD, COLOR_WALL, COLOR_UI, COLOR_HIGHLIGHT

### Snake Architecture (`snake.h`)

Linked-list structure: `snake_t` contains head/tail pointers to `snake_segment_t` nodes. Direction stored with separate `next_direction` for input buffering. Behavior injected via `snake_behavior_t*`.

### Game Board (`game.h:63-88`)

Board dimensions calculated at runtime based on terminal size (`game_calculate_board_size`). Minimum: 40x20. High score persisted to `data/highscore.txt`.

## Extensibility Points

1. **New snake behaviors**: Implement `snake_behavior_t` interface
2. **New food types**: Define `food_type_t` with custom `on_eaten` callback
3. **Custom rendering**: Implement `renderer_t` interface (e.g., for network streaming)
4. **New game states**: Add to `game_state_t`, implement `state_handler_t`
5. **New levels**: Add `level_config_t` entries with custom behavior/food combinations
