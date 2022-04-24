#include "ui.h"

#include "engine/window.h"

// Free function prototypes

// Label
void ui_label_free(UINode* node);

// Button
void ui_button_free(UINode* node);

// Input
void ui_input_free(UINode* node);

// Panel
void ui_panel_free(UINode* node);

// Statics
static const char* default_font_path_ = "res/font/FiraMono-Regular.ttf";
static Font* default_font_;
static Shader default_shader_;

// Static bool
static bool input_mode_;

// Init & terminate
void ui_init() {

    uint32_t font_size = (engine_window_get_retina()) ? FONT_DEFAULT_PIXEL_SIZE : 12;

    default_font_ = engine_font_new(
        default_font_path_,
        font_size,
        GL_LINEAR
    );

    default_shader_ = engine_shader_new("res/shader/ui.vert", "res/shader/ui.frag");
}

void ui_free() {

    engine_font_free(default_font_);
}

// Node
UINode* ui_node_new(vec2s pos, vec2s size) {

    // Allocate and initialize node
    UINode* n = (UINode*) malloc(sizeof(UINode));

    *n = (UINode) {
        .pos = pos,
        .rel_pos = (vec2s) {0, 0},
        .size = size,

        .visible = true,

        .element = NULL,
        .type = UI_TYPE_NONE,

        .parent = NULL,
    };

    // Allocate the list
    n->children = LIST_PTR_NEW(n->children, UINode);

    return n;
}

void ui_node_free(UINode* node) {

    // Free the element
    switch (node->type) {
    case UI_TYPE_NONE:
        break;
    case UI_TYPE_LABEL:
        ui_label_free(node);
        break;
    case UI_TYPE_BUTTON:
        ui_button_free(node);
        break;
    case UI_TYPE_INPUT:
        ui_input_free(node);
        break;
    case UI_TYPE_PANEL:
        ui_panel_free(node);
        break;
    default:
        printf(
            "ERROR: Failed to free node! Unknown node type, type was '%d'.\n", 
            node->type
        );
        abort();
        break;
    }

    // Destroy child
    for (int i = 0; i < node->children->count; ++i) {
        ui_node_free(LIST_GET(node->children, i));
    }

    // Free children list
    LIST_FREE(node->children);

    // Free the node
    free(node);
}

void ui_node_update_position(UINode* node) {

    // Update current position to parent if it exists
    if (node->parent) {
        glm_vec2_add(node->parent->pos.raw, node->rel_pos.raw, node->pos.raw);
    }
}

void ui_node_set_position(UINode* node, vec2s pos) {

    // Calculate position
    if (node->parent) {
        node->rel_pos = pos;
        glm_vec2_add(node->parent->pos.raw, node->rel_pos.raw, node->pos.raw);
    } else {
        node->pos = pos;
    }

    // Update the children
    for (int32_t i = 0; i < node->children->count; ++i) {
        ui_node_update_position(LIST_GET(node->children, i));
    }
}

// UI General
const Font* ui_default_font() {
    return (const Font*) default_font_;
}

const Shader ui_default_shader() {
    return default_shader_;
}

void ui_set_input_mode(bool value) {
    input_mode_ = value;
}

bool ui_get_input_mode() {
    return input_mode_;
}