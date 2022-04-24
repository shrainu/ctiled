#include "panel.h"

#include "label.h"
#include "button.h"
#include "input.h"

#include "engine/shader.h"
#include "engine/renderer.h"


// Defaults
#define PANEL_DEFAULT_PADDING       8.75
#define PANEL_DEFAULT_BAR_HEIGHT    24.0
#define PANEL_DEFAULT_MARGIN        7.5
#define PANEL_DEFAULT_LABEL         "Window"

// Constructor and destructor
UINode* ui_panel_new(const char* text, vec2s pos, vec2s size) {

    // Create the node
    UINode* node = ui_node_new(pos, size);

    // Create the panel
    UIPanel* panel = (UIPanel*) malloc(sizeof(UIPanel));

    // Set the node type and element
    node->type = UI_TYPE_PANEL;
    node->element = panel;

    // Initialize panel
    *panel = (UIPanel) {
        .movable = false,
        .element_offset = {
            PANEL_DEFAULT_MARGIN, 
            PANEL_DEFAULT_BAR_HEIGHT + PANEL_DEFAULT_MARGIN
        }
    };

    // Create child label
    const char* t = (text) ? text : PANEL_DEFAULT_LABEL;
    UINode* text_node = ui_label_new(t, (vec2s) {0, 0});

    // Add label to children
    text_node->parent = node;
    LIST_PUSH(node->children, text_node);

    // Center the label
    ui_label_set_alignment_to_parent(text_node, 0);

    // Adjust the label height
    float margin_y = (PANEL_DEFAULT_BAR_HEIGHT - text_node->size.y) / 2;
    ui_node_set_position(
        text_node, (vec2s) {
            text_node->rel_pos.x, 
            node->size.y - text_node->size.y - margin_y
        }
    );

    return node;
}

void ui_panel_free(UINode* node) {
    UIPanel* panel;
    if (!(panel = ui_panel_get(node))) {
        printf("ERROR: Given node is not a panel, type was '%d'.\n", node->type);
        abort();
    }

    // Free the panel
    free(panel);
}

// Update
void ui_panel_update(UINode* node) {
    UIPanel* panel;
    if (!(panel = ui_panel_get(node))) {
        printf("ERROR: Given node is not a panel, type was '%d'.\n", node->type);
        abort();
    }

    // Render
    Shader quad_shader = ui_default_shader();
    vec4 color;

    // Background
    color[0] = 0.35;
    color[1] = 0.35;
    color[2] = 0.35;
    color[3] = 1.0;

    engine_shader_bind(quad_shader);
    engine_shader_vec4(quad_shader, "u_color", color);

    engine_render_quad(NULL, NULL, (vec3) {node->pos.x, node->pos.y, -1.0}, node->size.raw);

    engine_shader_unbind(quad_shader);

    // Bar
    color[0] = 0.275;
    color[1] = 0.225;
    color[2] = 0.420;
    color[3] = 1.0;

    engine_shader_bind(quad_shader);
    engine_shader_vec4(quad_shader, "u_color", color);

    engine_render_quad(
        NULL, NULL,
        (vec3) {node->pos.x, node->pos.y + node->size.y - PANEL_DEFAULT_BAR_HEIGHT, -1.0},
        (vec2) {node->size.x, PANEL_DEFAULT_BAR_HEIGHT}
    );

    engine_shader_unbind(quad_shader);
    
    // Children update
    for (int32_t i = 0; i < node->children->count; ++i) {
        UINode* child = LIST_GET(node->children, i);
        
        switch (child->type) {
            case UI_TYPE_NONE:
                break;
            case UI_TYPE_LABEL:
                ui_label_update(child);
                break;
            case UI_TYPE_BUTTON:
                ui_button_update(child);
                break;
            case UI_TYPE_INPUT:
                ui_input_update(child);
                break;
            default:
                printf(
                    "ERROR: Failed to update node! Unknown node type '%d'.\n", 
                    child->type
                );
                break;
        };
    }
}

// Set
void ui_panel_add_node(UINode* node, UINode* child) {
    UIPanel* panel;
    if (!(panel = ui_panel_get(node))) {
        printf("ERROR: Given node is not a panel, type was '%d'.\n", node->type);
        abort();
    }

    // ADd child to panel's children
    child->parent = node;
    LIST_PUSH(node->children, child);
    
    // Set child pos
    ui_node_set_position(
        child, (vec2s) {
            panel->element_offset[0], 
            node->size.y - (panel->element_offset[1] + child->size.y)
        }
    );

    // Calculate the new offset
    panel->element_offset[1] += child->size.y + PANEL_DEFAULT_MARGIN;
}

// Get
UIPanel* ui_panel_get(UINode* node) {
    if(node->type != UI_TYPE_PANEL) {
        return NULL;
    }

    return node->element;
}