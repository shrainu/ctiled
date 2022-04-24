#pragma once

#include "util/common.h"

#include "ui.h"


// Type
typedef struct UIInput {
    LIST_TYPE(char) buffer;

    const char* place_holder;
    uint32_t max_input;
    float scale;

    bool cursor_over;
    bool on_focus;
} UIInput;

// Constructor and destructor
UINode* ui_input_new();

void ui_input_free();

// Update
void ui_input_update(UINode* node);

// Get
UIInput* ui_input_get(UINode* node);