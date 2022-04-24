#include "button.h"

#include "label.h"

#include "engine/renderer.h"
#include "engine/engine.h"
#include "engine/window.h"
#include "engine/input.h"


// Defaults
#define BUTTON_DEFAULT_SIZE     (vec2s) {80, 25}
#define BUTTON_DEFAULT_PADDING  8.75
#define BUTTON_DEFAULT_TEXT     "Click Me!"

// Construction & destruction
UINode* ui_button_new(const char* text, vec2s position, ui_btn_func_t callback) {

    // Create the node
    UINode* node = ui_node_new(position, BUTTON_DEFAULT_SIZE);

    // Create the button
    UIButton* button = (UIButton*) malloc(sizeof(UIButton));

    // Set the node type and element
    node->type = UI_TYPE_BUTTON;
    node->element = button;

    // Initialize the button
    *button = (UIButton) {
        .cursor_over = false,
        .clicked_on = false,

        .callback = callback
    };

    // Create child text
    const char* t = (text) ? text : BUTTON_DEFAULT_TEXT;
    UINode* text_node = ui_label_new(t, (vec2s) {0, 0});

    // Add label to children
    text_node->parent = node;
    LIST_PUSH(node->children, text_node);

    // Center the label
    ui_label_set_alignment_to_parent(text_node, 0);

    return node;
}

void ui_button_free(UINode* node) {
    UIButton* button;
    if (!(button = ui_button_get(node))) {
        printf("ERROR: Given node is not a button, type was '%d'.\n", node->type);
        abort();
    }

    // Free button
    free(node->element);
}

// Update
void ui_button_update(UINode* node) {
    UIButton* button;
    if (!(button = ui_button_get(node))) {
        printf("ERROR: Given node is not a button, type was '%d'.\n", node->type);
        abort();
    }

    // Update
    const double* cursor_pos = engine_input_get_cursor_pos();
    MouseButtonAction mba = engine_input_get_mouse_button(0);

    if ((cursor_pos[0] >= node->pos.x && cursor_pos[0] <= node->pos.x + node->size.x) &&
        (cursor_pos[1] >= node->pos.y && cursor_pos[1] <= node->pos.y + node->size.y)) {
        button->cursor_over = true;
    } else {
        button->cursor_over = false;
    }

    if (mba.just_pressed && button->cursor_over) {
        button->clicked_on = true;
    }

    if (mba.just_released && button->clicked_on && button->cursor_over) {
        button->clicked_on = false;
        (*button->callback)();
    } else if (mba.just_released && button->clicked_on) {
        button->clicked_on = false;
    }

    // Render
    vec3 render_pos = {
        node->pos.x,
        node->pos.y,
        -1.0
    };

    vec2 size = {
        node->size.x,
        node->size.y
    };

    if (button->clicked_on) {
        render_pos[0] += 1;
        render_pos[1] += 1;
        size[0] -= 2;
        size[1] -= 2;
    }

    vec4 color;
    if (button->cursor_over) {
        color[0] = 0.325;
        color[1] = 0.275;
        color[2] = 0.470;
        color[3] = 1.0;
    } else {
        color[0] = 0.275;
        color[1] = 0.225;
        color[2] = 0.420;
        color[3] = 1.0;
    }

    Shader quad_shader = ui_default_shader();

    engine_shader_bind(quad_shader);
    engine_shader_vec4(quad_shader, "u_color", color);

    engine_render_quad(NULL, NULL, render_pos, size);

    engine_shader_unbind(quad_shader);

    // Update the children
    UINode* label = LIST_GET(node->children, 0);
    ui_label_update(label);
}

// Get
UIButton* ui_button_get(UINode* node) {
    if (node->type != UI_TYPE_BUTTON) {
        return NULL;
    }

    return (UIButton*) node->element;
}