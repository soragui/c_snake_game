# Snake Game Implementation Plan

## Overview
Build a console Snake game in C using ncurses with modular architecture and function pointers for extensibility.

## Architecture Summary
- Modular design with separate .c/.h file pairs
- Function pointer interfaces for extensibility
- State machine pattern for game screens
- Adaptive terminal size support

## Implementation Steps

### Phase 1: Foundation & Core Systems

#### Step 1: Project Structure Setup
- Create directory structure with src/ folder
- Set up Makefile with ncurses linking
- Create all header files with basic structures

#### Step 2: Utility Layer (utils.c/h)
- Random number generation
- Timer utilities for game speed
- Terminal size detection
- Color definitions and constants
- Basic data structures (point_t, direction_t)

#### Step 3: Core Game State (game.c/h)
- Game state enumeration
- Function pointer interfaces definition
- Game context structure
- State machine framework
- Level configuration system

### Phase 2: Game Entities

#### Step 4: Snake Implementation (snake.c/h)
- Snake data structure (linked list of positions)
- Movement behavior functions
- Collision detection functions
- Growth mechanism
- Function pointer interface for snake behaviors

#### Step 5: Food System (food.c/h)
- Food placement algorithm (avoid snake body)
- Different food types with function pointers
- Food consumption logic
- Random food generation

### Phase 3: User Interface

#### Step 6: ncurses UI Foundation (ui.c/h)
- ncurses initialization and cleanup
- Color pair setup
- Screen clearing and border drawing
- Function pointer interface for different screens

#### Step 7: Screen Implementations
- Start screen with level selection menu
- Game screen with adaptive board size
- Score display area
- Game over screen with replay option

#### Step 8: Input Handling (input.c/h)
- Keyboard input capture
- Arrow key mapping
- Input validation
- Function pointer interface for different input modes

### Phase 4: Game Logic Integration

#### Step 9: Score System (score.c/h)
- Score calculation with level multipliers
- High score persistence to file
- Score display formatting

#### Step 10: Main Game Loop (main.c)
- Initialize all systems
- Main game loop with timing
- State transitions
- Cleanup and exit handling

### Phase 5: Polish & Features

#### Step 11: Level System
- Multiple speed levels implementation
- Level selection menu
- Dynamic difficulty adjustment

#### Step 12: Game Features
- Collision detection refinement
- Smooth gameplay timing
- Pause functionality
- Graceful exit handling

#### Step 13: Visual Polish
- Unicode character implementation
- Color scheme application
- Responsive UI layout
- Border and decoration elements

## Technical Specifications

### Function Pointer Interfaces
```c
// Game State Handlers
typedef struct {
    void (*update)(void);
    void (*render)(void);
    void (*handle_input)(int key);
} state_handler_t;

// Snake Behaviors
typedef struct {
    void (*move)(snake_t* snake);
    bool (*check_collision)(snake_t* snake);
    void (*grow)(snake_t* snake);
} snake_behavior_t;

// Food Types
typedef struct {
    int value;
    char symbol;
    int color;
    void (*on_eaten)(game_t* game);
} food_type_t;
```

### Key Data Structures
```c
typedef struct snake_segment {
    int x, y;
    struct snake_segment* next;
} snake_segment_t;

typedef struct {
    snake_segment_t* head;
    snake_segment_t* tail;
    direction_t direction;
    int length;
    snake_behavior_t* behavior;
} snake_t;

typedef struct {
    game_state_t state;
    snake_t* snake;
    food_t* food;
    int score;
    int high_score;
    int level;
    int board_width;
    int board_height;
    state_handler_t* current_handler;
} game_t;
```

### Build System
- Makefile with ncurses linking (-lncurses)
- Debug and release configurations
- Clean and install targets

### File I/O
- High score persistence in data/highscore.txt
- Error handling for file operations

## Testing Strategy
- Unit tests for core algorithms
- Integration tests for game flow
- Manual testing for UI responsiveness
- Terminal size compatibility testing

## Success Criteria
- ✓ Smooth snake movement with arrow keys
- ✓ Proper collision detection (walls, self)
- ✓ Snake growth on food consumption
- ✓ Multiple difficulty levels
- ✓ Score tracking with persistence
- ✓ Responsive to terminal size changes
- ✓ Beautiful Unicode-based UI
- ✓ Clean modular architecture
- ✓ Extensible via function pointers