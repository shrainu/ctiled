#include "input.h"

#include "engine/renderer.h"
#include "engine/window.h"
#include "engine/input.h"

// Defaults
#define INPUT_DEFAULT_SCALE         1.0
#define INPUT_DEFAULT_RETINA_SCALE  0.25
#define INPUT_DEFAULT_MAX_INPUT     255
#define INPUT_DEFAULT_SIZE          (vec2s) {100, 25}
#define INPUT_DEFAULT_PLACE_HOLDER  "Input"
#define INPUT_DEFAULT_COLOR         (vec4s) {1, 1, 1, 1}

// Constructor and destructor
UINode* ui_input_new(const char* placeholder, vec2s pos) {

    // Create the node
    UINode* node = ui_node_new(pos, INPUT_DEFAULT_SIZE);

    // Create the input
    UIInput* input = (UIInput*) malloc(sizeof(UIInput));

    // Set node type and element
    node->type = UI_TYPE_INPUT;
    node->element = input;

    // Initialize the input
    *input = (UIInput) {
        .place_holder = (placeholder) ? placeholder : INPUT_DEFAULT_PLACE_HOLDER,
        .max_input = INPUT_DEFAULT_MAX_INPUT,
        .scale = (engine_window_get_retina()) ? INPUT_DEFAULT_RETINA_SCALE : INPUT_DEFAULT_SCALE,

        .cursor_over = false,
        .on_focus = false
    };
    input->buffer = LIST_NEW(input->buffer, char);

    return node;
}

void ui_input_free(UINode* node) {
    UIInput* input;
    if (!(input = ui_input_get(node))) {
        printf("ERROR: Given node is not a input, type was '%d'.\n", node->type);
        abort();
    }

    // Free the buffer
    LIST_FREE(input->buffer);

    // Free the input
    free(input);
}

// Update
void ui_input_update(UINode* node) {
    UIInput* input;
    if (!(input = ui_input_get(node))) {
        printf("ERROR: Given node is not a input, type was '%d'.\n", node->type);
        abort();
    }

    // Update
    const double* cursor_pos = engine_input_get_cursor_pos();
    MouseButtonAction mba = engine_input_get_mouse_button(0);

    if ((cursor_pos[0] >= node->pos.x && cursor_pos[0] <= node->pos.x + node->size.x) &&
        (cursor_pos[1] >= node->pos.y && cursor_pos[1] <= node->pos.y + node->size.y)) {
        input->cursor_over = true;
    } else {
        input->cursor_over = false;
    }

    if (mba.just_pressed && input->cursor_over) {
        input->on_focus = true;
    } else if (mba.just_pressed) {
        input->on_focus = false;
    }

    if (input->on_focus) {

        ui_set_input_mode(true);

        LIST_TYPE(char) char_pressed = engine_input_get_chars_pressed();
        LIST_TYPE(KeyAction) key_pressed = engine_input_get_keys_pressed();

        for (int32_t i = 0; i < char_pressed->count; ++i) {
            LIST_PUSH(input->buffer, LIST_GET(char_pressed, i));
        }

        for (int32_t i = 0; i < key_pressed->count; ++i) {
            const KeyAction* key = &LIST_GET(key_pressed, i);

            if (key->key == GLFW_KEY_BACKSPACE && 
                (key->state == INPUT_KEY_PRESS || key->state == INPUT_KEY_REPEAT)) {
                
                if (input->buffer->count == 0) {
                    break;
                }

                input->buffer->array[input->buffer->count - 1] = '\0';
                input->buffer->count--;
            }
        }

        if (input->buffer->count != input->buffer->capacity) {

            input->buffer->array[input->buffer->count] = '\0';
        } else {
            input->buffer->capacity *= 2;
            input->buffer->array = (char*) realloc(
                input->buffer->array, 
                sizeof(char) * input->buffer->capacity
            );

            input->buffer->array[input->buffer->count] = '\0';
        }
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

    vec4 color = {0.275, 0.225, 0.420, 1.0};

    Shader quad_shader = ui_default_shader();

    engine_shader_bind(quad_shader);
    engine_shader_vec4(quad_shader, "u_color", color);

    engine_render_quad(NULL, NULL, render_pos, size);

    engine_shader_unbind(quad_shader);

    // Render the buffer
    const char* text = (input->buffer->count) ? input->buffer->array : input->place_holder;

    vec4s text_color = (input->buffer->count) ? INPUT_DEFAULT_COLOR : (vec4s) {0.6, 0.6, 0.6, 1};

    vec2s text_size = engine_font_get_text_size(
        (Font*) ui_default_font(), 
        text, 
        input->scale
    );

    vec2 text_margin = {
        (node->size.x - text_size.x) / 2,
        (node->size.y - text_size.y) / 2
    };

    render_pos[0] += text_margin[0];
    render_pos[1] += text_margin[1];

    engine_render_text(
        (Font*) ui_default_font(), 
        render_pos, 
        text, 
        text_color.raw,
        input->scale
    );
}

// Get
UIInput* ui_input_get(UINode* node) {
    if (node->type != UI_TYPE_INPUT) {
        return NULL;
    }

    return (UIInput*) node->element;
}