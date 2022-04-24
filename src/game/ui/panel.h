#pragma once

#include "util/common.h"

#include "ui.h"


// Type
typedef struct UIPanel {
    bool movable;
    vec2 element_offset;
} UIPanel;

// Constructor & destructor
UINode* ui_panel_new(const char* text, vec2s pos, vec2s size);

void ui_panel_free(UINode* node);

// Update
void ui_panel_update(UINode* node);

// Set
void ui_panel_add_node(UINode* node, UINode* child);

// Get
UIPanel* ui_panel_get(UINode* node);