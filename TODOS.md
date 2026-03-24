# TODOS.md

Deferred work and future improvements for the snake game project.

---

## P2 - Replay Playback UI

**What:** Add replay playback functionality with menu UI and controls

**Why:** The replay system currently records gameplay but users cannot watch replays. Playback requires:
- Replay menu option (view saved replays)
- File browser to select replay file
- Playback controls (play, pause, stop, speed control)
- Injecting replay inputs into game loop during playback

**Pros:**
- Complete the replay feature end-to-end
- Cool demo capability for sharing impressive gameplay
- Educational value for teaching deterministic simulation

**Cons:**
- Requires significant UI work (file browser, playback UI)
- Must handle edge cases (replay desync, modified level config)
- ~3 hours human effort

**Context:** Recording integration completed in engineering review (2026-03-24). `replay_get_instance()` returns recorded replays but no UI exists to trigger playback. The `replay_get_next_input()` function exists but is never called.

**Depends on:** None

**Priority:** P2

---

## P3 - Key Binding Parse Error Logging

**What:** Add error logging when keybindings.cfg fails to parse

**Why:** `input_load_bindings()` silently skips invalid config lines. Users get no feedback if their custom bindings are ignored.

**Pros:**
- Better user experience for config errors
- Easier debugging of config file issues
- Demonstrates proper error handling for tutorial project

**Cons:**
- Minor polish; silent fallback works
- Adds fprintf calls throughout config parser

**Context:** `input.c:84-106` uses `sscanf()` and skips lines that don't match `%31[^=]=%d` format. No stderr output on parse failure.

**Depends on:** None

**Priority:** P3

---

## P3 - Wraparound Level Mazes

**What:** Add obstacle/maze support for wraparound levels (6-10)

**Why:** Currently only classic levels (1-5) have mazes. Wraparound levels use the same maze data structure but `maze_has_obstacles()` returns false for levels 6-10.

**Pros:**
- More variety in high-difficulty wraparound modes
- Consistent feature parity between classic and wraparound modes
- Could create interesting "wraparound maze" puzzles

**Cons:**
- Requires designing wraparound-compatible maze layouts
- Obstacle coordinates need to account for wrap behavior
- Lower priority than core gameplay features

**Context:** `obstacle.c:22-81` initializes mazes only for levels 1-5. `maze_has_obstacles()` at line 190-192 returns `level >= 3 && level <= 5`.

**Depends on:** None

**Priority:** P3

---

## P2 - 2-Player Food Collision Fix

**What:** Fix food spawning on player 2's snake in 2-player mode

**Status:** ✅ FIXED in engineering review (2026-03-24)

**Context:** `food_is_position_valid()` originally only checked `game->snake`, not `game->snake2`. Fixed by adding check for player 2's snake when `game->two_player` is true.

---

## P2 - Obstacle Rendering Coordinate Transform

**What:** Fix obstacle rendering to use absolute screen coordinates

**Status:** ✅ FIXED in engineering review (2026-03-24)

**Context:** `obstacle_render()` originally drew obstacles at relative coordinates, causing them to render at wrong positions. Fixed by accepting `board_offset_x` and `board_offset_y` parameters and transforming coordinates.

---

## P1 - Replay System Integration

**What:** Wire replay recording into game loop

**Status:** ✅ FIXED in engineering review (2026-03-24)

**Context:** Replay module existed but was never called. Fixed by:
- Adding `#include "replay.h"` to game.c and ui.c
- Calling `replay_start_recording()` in `game_change_level()`
- Calling `replay_record_frame()` in `game_screen_handle_input()`
- Calling `replay_stop_recording()` in `game_screen_exit()`

---

## P1 - Snake Movement Memory Optimization

**What:** Reuse tail segment instead of malloc/free every frame

**Status:** ✅ FIXED in engineering review (2026-03-24)

**Context:** `snake_move_normal()` allocated new head and freed tail on every frame. Refactored to move tail segment to head position, eliminating per-frame allocations. Growth path still allocates (unavoidable).
