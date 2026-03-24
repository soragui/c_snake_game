#ifndef REPLAY_H
#define REPLAY_H

#include "game.h"
#include <stdbool.h>

#define MAX_REPLAY_FRAMES 10000
#define MAX_REPLAY_FILENAME 256

// Recorded input frame
typedef struct {
    int key;
    int frame_number;
} replay_frame_t;

// Replay structure
typedef struct {
    replay_frame_t frames[MAX_REPLAY_FRAMES];
    int num_frames;
    int current_frame;
    bool is_recording;
    bool is_playing;
    int initial_level;
    bool two_player;
    char filename[MAX_REPLAY_FILENAME];
} replay_t;

// Replay management
void replay_init(replay_t* replay);
void replay_destroy(replay_t* replay);

// Recording functions
void replay_start_recording(replay_t* replay, int level, bool two_player);
void replay_record_frame(replay_t* replay, int key, int frame_number);
void replay_stop_recording(replay_t* replay);

// Playback functions
void replay_start_playback(replay_t* replay);
int replay_get_next_input(replay_t* replay, int current_frame);
bool replay_is_playing(replay_t* replay);
void replay_stop_playback(replay_t* replay);

// File I/O
bool replay_save_to_file(replay_t* replay, const char* filename);
bool replay_load_from_file(replay_t* replay, const char* filename);

// Global replay instance
replay_t* replay_get_instance(void);

#endif // REPLAY_H
