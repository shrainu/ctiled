#include "label.h"

#include "engine/renderer.h"
#include "engine/window.h"


// Defaults
#define LABEL_DEFAULT_SCALE         1.0
#define LABEL_DEFAULT_RETINA_SCALE  0.25
#define LABEL_DEFAULT_COLOR         (vec4s) {1.0, 1.0, 1.0, 1.0}

// Construction and destruction
UINode* ui_label_new(const char* text, vec2s pos) {

    // Create the node
    UINode* node = ui_node_new(pos, (vec2s) {0, 0});

    // Create the label
    UILabel* label = (UILabel*) malloc(sizeof(UILabel));

    // Set the node type and element
    node->type = UI_TYPE_LABEL;
    node->element = label;

    // Initialize the element
    *label = (UILabel) {
        .scale = (engine_window_get_retina()) ? LABEL_DEFAULT_RETINA_SCALE : LABEL_DEFAULT_SCALE,
        .color = LABEL_DEFAULT_COLOR
    };

    // Set the text & Calculate size
    ui_label_set_text(node, text);

    return node;
}

void ui_label_free(UINode* node) {
    UILabel* label;
    if (!(label = ui_label_get(node))) {
        printf("ERROR: Given node is not a label, type was '%d'.\n", node->type);
        abort();
    }

    // Free components
    free(label->buffer);

    label->len = 0;
    label->capacity = 0;

    // Free the label
    free(label);
}

// Update
void ui_label_update(UINode* node) {
    UILabel* label;
    if (!(label = ui_label_get(node))) {
        printf("ERROR: Given node is not a label, type was '%d'.\n", node->type);
        abort();
    }

    // Update

    // Render
    vec3 render_pos = {
        node->pos.x,
        node->pos.y,
        -1.0
    };

    engine_render_text(
        (Font*) ui_default_font(), 
        render_pos, 
        label->buffer, 
        label->color.raw, 
        label->scale
    );
}

// Set
void ui_label_calculate_size(UINode* node) {
    UILabel* label;
    if (!(label = ui_label_get(node))) {
        printf("ERROR: Given node is not a label, type was '%d'.\n", node->type);
        abort();
    }

    node->size = engine_font_get_text_size(
        (Font*) ui_default_font(), 
        label->buffer, 
        label->scale
    );
}

void ui_label_set_text(UINode* node, const char* text) {
    UILabel* label;
    if (!(label = ui_label_get(node))) {
        printf("ERROR: Given node is not a label, type was '%d'.\n", node->type);
        abort();
    }

    size_t len = strlen(text) + 1; // +1 for null terminator
    if (len == 1) {
        return;
    }

    if (len > label->capacity) {
        label->capacity = len;

        free(label->buffer);
        label->buffer = (char*) calloc(label->capacity, sizeof(char));
    }

    strcpy(label->buffer, text);
    label->buffer[len - 1] = '\0';

    // Calculate the new size
    ui_label_calculate_size(node);
}

void ui_label_set_alignment_to_parent(UINode* node, int32_t alignment) {
    UILabel* label;
    if (!(label = ui_label_get(node))) {
        printf("ERROR: Given node is not a label, type was '%d'.\n", node->type);
        abort();
    }

    UINode* parent;
    if (!(parent = node->parent)) {
        return;
    }

    /* Alignment is currently useless */

    float x_margin = (parent->size.x - node->size.x) / 2;
    float y_margin = (parent->size.y - node->size.y) / 2;

    ui_node_set_position(node, (vec2s) {x_margin, y_margin});
}

// Get
UILabel* ui_label_get(UINode* node) {
    if (node->type != UI_TYPE_LABEL) {
        return NULL;
    }

    return (UILabel*) node->element;
}

vec2s ui_label_get_size(UINode* node) {
    UILabel* label;
    if (!(label = ui_label_get(node))) {
        printf("ERROR: Given node is not a label, type was '%d'.\n", node->type);
        abort();
    }

    return node->size;
}

const char* ui_label_get_text(UINode* node) {
    UILabel* label;
    if (!(label = ui_label_get(node))) {
        printf("ERROR: Given node is not a label, type was '%d'.\n", node->type);
        abort();
    }

    return (const char*) label->buffer;
}