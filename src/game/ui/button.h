#pragma once

#include "util/common.h"

#include "ui.h"

// Typedefs
typedef void (*ui_btn_func_t) ();

// Button
typedef struct UIButton {
    bool cursor_over;
    bool clicked_on;

    void (*callback) ();
} UIButton;

// Construction & destruction
UINode* ui_button_new(const char* text, vec2s position, ui_btn_func_t callback);

void ui_button_free(UINode* node);

// Update
void ui_button_update(UINode* node);

// Get
UIButton* ui_button_get(UINode* node);