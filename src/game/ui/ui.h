#pragma once

#include "util/common.h"
#include "util/list.h"

#include "engine/font.h"
#include "engine/shader.h"

// Type definitons
#define UI_TYPE_NONE    -1
#define UI_TYPE_LABEL    0
#define UI_TYPE_BUTTON   1
#define UI_TYPE_INPUT    2
#define UI_TYPE_PANEL    3

// Typedef
typedef int32_t node_t;

// Node
typedef struct UINode UINode;

LIST_PTR_DECLARE(UINode);

struct UINode {
    vec2s pos;
    vec2s rel_pos;
    vec2s size;

    bool visible;

    void* element;
    node_t type;

    struct UINode* parent;
    LIST_PTR_TYPE(UINode) children;
};

// Creation and termination
UINode* ui_node_new(vec2s pos, vec2s size);

void ui_node_free(UINode* node);

void ui_node_update_position(UINode* node);

void ui_node_set_position(UINode* node, vec2s pos);

// UI General
const Font* ui_default_font();

const Shader ui_default_shader();

void ui_set_input_mode(bool value);

bool ui_get_input_mode();

// Init & terminate
void ui_init();

void ui_free();