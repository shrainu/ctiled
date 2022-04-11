#include "scene.h"

// Active scene properties
static uint32_t next_scene_id_;
static uint32_t active_scene_id_ = -1;

// Get
uint32_t game_scene_get_new_id() {
    return active_scene_id_++;
}

uint32_t game_scene_get_active_scene() {
    return active_scene_id_;
}

// Set
void game_scene_set_active_scene(uint32_t id) {
    active_scene_id_ = id;
}