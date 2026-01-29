# Snake Game

A classic Snake game implemented in C using ncurses, featuring a modular architecture with function pointers for extensibility.

## Features

- **Classic Snake Gameplay**: Control the snake with arrow keys, eat food to grow, avoid walls and self-collision
- **Multiple Difficulty Levels**: 5 levels from Easy to Extreme with different speeds and score multipliers
- **Beautiful Unicode UI**: Modern console interface with colors and Unicode characters
- **Adaptive Display**: Automatically adjusts to terminal size
- **Score System**: Current score, high score persistence, and level-based scoring
- **Modular Architecture**: Clean separation of concerns with function pointer interfaces

## Requirements

- C compiler (gcc recommended)
- ncurses library
- Terminal with Unicode support
- Minimum terminal size: 40x20

## Installation

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential libncurses5-dev
```

### Fedora/CentOS/RHEL
```bash
sudo dnf install gcc ncurses-devel make
```

### macOS
```bash
brew install ncurses
```

## Building

1. Clone or download the project
2. Navigate to the project directory
3. Build the game:

```bash
# Check dependencies
make deps

# Build release version (optimized)
make

# Or build debug version
make debug

# Run the game
make run
```

## How to Play

### Controls
- **Arrow Keys** or **WASD**: Move the snake
- **ENTER/SPACE**: Select menu items or restart game
- **P/SPACE**: Pause/unpause during gameplay
- **ESC/Q**: Return to menu or quit
- **R**: Restart game (on game over screen)
- **M**: Return to main menu (on game over screen)

### Gameplay
1. Start the game and select difficulty level
2. Control the snake to eat red food items (*)
3. Snake grows longer with each food eaten
4. Avoid hitting walls or the snake's own body
5. Try to achieve the highest score possible

### Scoring
- Each food item gives base points (10) multiplied by difficulty level
- Higher levels provide better score multipliers:
  - Level 1 (Easy): 1x multiplier
  - Level 2 (Medium): 2x multiplier
  - Level 3 (Hard): 3x multiplier
  - Level 4 (Very Hard): 4x multiplier
  - Level 5 (Extreme): 5x multiplier

## Architecture

The game follows a modular design with clear separation of concerns:

### Core Modules
- **main.c**: Entry point and main game loop
- **game.c/h**: Game state management and core logic
- **snake.c/h**: Snake entity with behavior system
- **food.c/h**: Food generation and consumption
- **ui.c/h**: ncurses-based rendering system
- **input.c/h**: Keyboard input handling
- **score.c/h**: Score calculation and persistence
- **utils.c/h**: Utility functions and common types

### Design Patterns
- **State Machine**: Game states (start screen, playing, game over)
- **Function Pointers**: Extensible behavior systems
- **Strategy Pattern**: Different snake behaviors and food types
- **Observer Pattern**: State change handlers

### Extensibility
The function pointer architecture makes it easy to add:
- New food types with different behaviors
- Different snake movement patterns
- Custom game modes and rules
- New rendering backends
- Alternative input methods

## File Structure

```
snake_game/
├── src/                   # Source code
│   ├── main.c             # Entry point
│   ├── game.c/h           # Game state management
│   ├── snake.c/h          # Snake entity
│   ├── food.c/h           # Food system
│   ├── ui.c/h             # User interface
│   ├── input.c/h          # Input handling
│   ├── score.c/h          # Score system
│   └── utils.c/h          # Utilities
├── data/                  # Game data (high scores)
├── obj/                   # Build objects (created automatically)
├── Makefile              # Build configuration
├── README.md             # This file
└── implementation_plan.md # Development plan
```

## Development

### Building for Development
```bash
# Debug build with symbols
make debug

# Clean build artifacts
make clean

# Install system-wide (optional)
sudo make install
```

### Code Style
- C99 standard
- Snake_case naming convention
- Comprehensive error checking
- Memory management with proper cleanup
- Modular design with clear interfaces

## Troubleshooting

### Common Issues

**Terminal too small**
- Resize terminal to at least 40x20 characters
- The game will display current and required dimensions

**Colors not working**
- Ensure your terminal supports colors
- Try a modern terminal emulator like gnome-terminal or iTerm2

**Build errors**
- Install ncurses development headers
- Ensure gcc is installed and accessible

**Game runs slowly**
- Try a different terminal emulator
- Close other applications to free up system resources

### Performance Tips
- Use a hardware-accelerated terminal if available
- Ensure terminal font supports Unicode characters
- For best experience, use a terminal with at least 256 color support

## Contributing

This project is designed with extensibility in mind. Some ideas for contributions:
- New food types (bonus items, penalties, power-ups)
- Different snake behaviors (wrap-around walls, variable speed)
- Enhanced graphics (more Unicode characters, animations)
- Sound effects (terminal beeps and alerts)
- Configuration file support
- Multiplayer support
- AI snake opponents

## License

This project is provided as-is for educational purposes.

## Credits

Developed with a focus on clean architecture, maintainability, and extensibility using modern C programming practices.