# Changelog

All notable changes to this project will be documented in this file.

## [0.1.1.0] - 2026-03-24

### Added
- 4 food types: apple (standard), bonus (+25 points), poison (-15 points/shrink), speed (+15 points + bonus)
- Wall-crossing mode (levels 6-10) with wraparound snake behavior
- Maze/obstacle system for levels 3-5 with 4, 8, and 12 obstacles respectively
- 2-player local multiplayer mode with independent snakes and scores
- Configurable key bindings with file persistence (`data/keybindings.cfg`)
- Game replay recording system (records input frames to `replays/` directory)
- Pause screen UI with resume/quit options

### Changed
- Snake movement optimized to reuse tail segment instead of malloc/free every frame
- Food spawning now checks both snakes' positions in 2-player mode
- Obstacle rendering transformed to use absolute screen coordinates

### Fixed
- Replay system integration (now properly wired into game loop)
- Food spawning on player 2's snake in 2-player mode
- Obstacle rendering at incorrect positions
