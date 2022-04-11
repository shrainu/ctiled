#pragma once

#include "util/common.h"


// Typedefs
typedef int32_t (*scene_func_t) ();

// Defines
#define SCENE_EXECUTED  0
#define SCENE_SKIPPED   1

// Shared scene properties and function declerations
#define SCENE_DECLARE(prefix) \
    void game_scene_##prefix##_set_active(); \
    \
    void game_scene_##prefix##_load(); \
    void game_scene_##prefix##_free(); \
    int32_t game_scene_##prefix()

#define SCENE_DEFINE(prefix) \
    static uint32_t scene_id_; \
    \
    void game_scene_##prefix##_set_active() { game_scene_set_active_scene(scene_id_); } \

// Get
uint32_t game_scene_get_new_id();

uint32_t game_scene_get_active_scene();

// Set
void game_scene_set_active_scene(uint32_t id);