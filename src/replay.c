#include "replay.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Global replay instance
static replay_t g_replay;
static int replay_initialized = 0;

/******************************************************************************
 * @brief 初始化回放系统
 *
 * @param replay 回放结构体指针
 *****************************************************************************/
void replay_init(replay_t* replay) {
    if (!replay) return;

    replay->num_frames = 0;
    replay->current_frame = 0;
    replay->is_recording = false;
    replay->is_playing = false;
    replay->initial_level = 1;
    replay->two_player = false;
    replay->filename[0] = '\0';
}

/******************************************************************************
 * @brief 销毁回放系统
 *
 * @param replay 回放结构体指针
 *****************************************************************************/
void replay_destroy(replay_t* replay) {
    if (!replay) return;
    replay->num_frames = 0;
    replay->is_recording = false;
    replay->is_playing = false;
}

/******************************************************************************
 * @brief 开始录制回放
 *
 * @param replay 回放结构体指针
 * @param level 初始等级
 * @param two_player 是否为双玩家模式
 *****************************************************************************/
void replay_start_recording(replay_t* replay, int level, bool two_player) {
    if (!replay) return;

    replay->num_frames = 0;
    replay->current_frame = 0;
    replay->is_recording = true;
    replay->is_playing = false;
    replay->initial_level = level;
    replay->two_player = two_player;
}

/******************************************************************************
 * @brief 录制一帧输入
 *
 * @param replay 回放结构体指针
 * @param key 按键值
 * @param frame_number 帧号
 *****************************************************************************/
void replay_record_frame(replay_t* replay, int key, int frame_number) {
    if (!replay || !replay->is_recording) return;

    if (replay->num_frames >= MAX_REPLAY_FRAMES) return;

    replay->frames[replay->num_frames].key = key;
    replay->frames[replay->num_frames].frame_number = frame_number;
    replay->num_frames++;
}

/******************************************************************************
 * @brief 停止录制回放
 *
 * @param replay 回放结构体指针
 *****************************************************************************/
void replay_stop_recording(replay_t* replay) {
    if (!replay) return;
    replay->is_recording = false;
}

/******************************************************************************
 * @brief 开始播放回放
 *
 * @param replay 回放结构体指针
 *****************************************************************************/
void replay_start_playback(replay_t* replay) {
    if (!replay || replay->num_frames == 0) return;

    replay->current_frame = 0;
    replay->is_playing = true;
    replay->is_recording = false;
}

/******************************************************************************
 * @brief 获取下一帧的输入
 *
 * @param replay 回放结构体指针
 * @param current_frame 当前帧号
 * @return int 按键值，如果没有更多帧返回 -1
 *****************************************************************************/
int replay_get_next_input(replay_t* replay, int current_frame) {
    if (!replay || !replay->is_playing) return -1;

    if (replay->current_frame >= replay->num_frames) {
        replay->is_playing = false;
        return -1;
    }

    replay_frame_t* frame = &replay->frames[replay->current_frame];

    // Check if we're at the right frame
    if (frame->frame_number <= current_frame) {
        replay->current_frame++;
        return frame->key;
    }

    return -1;  // Not yet time for this frame
}

/******************************************************************************
 * @brief 检查是否正在播放
 *
 * @param replay 回放结构体指针
 * @return bool 正在播放返回 true
 *****************************************************************************/
bool replay_is_playing(replay_t* replay) {
    if (!replay) return false;
    return replay->is_playing;
}

/******************************************************************************
 * @brief 停止播放回放
 *
 * @param replay 回放结构体指针
 *****************************************************************************/
void replay_stop_playback(replay_t* replay) {
    if (!replay) return;
    replay->is_playing = false;
    replay->current_frame = 0;
}

/******************************************************************************
 * @brief 保存回放文件
 *
 * 文件格式：
 * - 第一行：等级，双玩家标志
 * - 后续行：帧号，按键值
 *
 * @param replay 回放结构体指针
 * @param filename 文件名
 * @return bool 成功返回 true
 *****************************************************************************/
bool replay_save_to_file(replay_t* replay, const char* filename) {
    if (!replay || !filename || replay->num_frames == 0) return false;

    FILE* file = fopen(filename, "w");
    if (!file) return false;

    // Write header
    fprintf(file, "# Snake Game Replay\n");
    fprintf(file, "level=%d\n", replay->initial_level);
    fprintf(file, "two_player=%d\n", replay->two_player ? 1 : 0);
    fprintf(file, "frames=%d\n", replay->num_frames);
    fprintf(file, "\n");

    // Write frames
    for (int i = 0; i < replay->num_frames; i++) {
        fprintf(file, "%d,%d\n", replay->frames[i].frame_number, replay->frames[i].key);
    }

    fclose(file);
    return true;
}

/******************************************************************************
 * @brief 从文件加载回放
 *
 * @param replay 回放结构体指针
 * @param filename 文件名
 * @return bool 成功返回 true
 *****************************************************************************/
bool replay_load_from_file(replay_t* replay, const char* filename) {
    if (!replay || !filename) return false;

    FILE* file = fopen(filename, "r");
    if (!file) return false;

    replay_init(replay);

    char line[64];
    int frames_count = 0;

    // Parse header
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;

        int value;
        if (sscanf(line, "level=%d", &value) == 1) {
            replay->initial_level = value;
        } else if (sscanf(line, "two_player=%d", &value) == 1) {
            replay->two_player = (value != 0);
        } else if (sscanf(line, "frames=%d", &frames_count) == 1) {
            // Found frames count, now read frames
            break;
        }
    }

    // Read frames
    int frame_num, key;
    while (replay->num_frames < frames_count &&
           fscanf(file, "%d,%d", &frame_num, &key) == 2) {
        if (replay->num_frames < MAX_REPLAY_FRAMES) {
            replay->frames[replay->num_frames].frame_number = frame_num;
            replay->frames[replay->num_frames].key = key;
            replay->num_frames++;
        }
    }

    fclose(file);

    // Copy filename
    strncpy(replay->filename, filename, MAX_REPLAY_FILENAME - 1);
    replay->filename[MAX_REPLAY_FILENAME - 1] = '\0';

    return replay->num_frames > 0;
}

/******************************************************************************
 * @brief 获取全局回放实例
 *
 * @return replay_t* 回放实例指针
 *****************************************************************************/
replay_t* replay_get_instance(void) {
    if (!replay_initialized) {
        replay_init(&g_replay);
        replay_initialized = 1;
    }
    return &g_replay;
}
