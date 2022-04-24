#pragma once

#include "util/common.h"

#include "ui.h"

// Label
typedef struct UILabel {
    char* buffer;
    uint32_t len;
    uint32_t capacity;

    float scale;
    vec4s color;
} UILabel;

// Constructor & destructor
UINode* ui_label_new(const char* text, vec2s pos);

void ui_label_free(UINode* node);

// Update
void ui_label_update(UINode* node);

// Set
void ui_label_calculate_size(UINode* node);

void ui_label_set_text(UINode* node, const char* text);

void ui_label_set_alignment_to_parent(UINode* node, int32_t alignment);

// Get
UILabel* ui_label_get(UINode* node);

vec2s ui_label_get_size(UINode* node);

const char* ui_label_get_text(UINode* node);