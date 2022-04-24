#pragma once

#include "util/common.h"
#include "util/list.h"

// Max mouse button allowed
#define INPUT_MAX_MOUSE_BUTTON 8

// Mouse button action
typedef struct MouseButtonAction {
    bool pressed;
    bool just_pressed;
    bool just_released;
} MouseButtonAction;

// Mouse button Input
typedef struct MouseButtonInput {
    MouseButtonAction buttons[INPUT_MAX_MOUSE_BUTTON];
} MouseButtonInput;

// Key states
#define INPUT_KEY_RELEASE   0
#define INPUT_KEY_PRESS     1
#define INPUT_KEY_REPEAT    2

// Key action
typedef struct KeyAction {
    int32_t key;
    int32_t state;
} KeyAction;

// List definitons
LIST_DECLARE(KeyAction);

// Callbacks
void engine_input_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void engine_input_char_input_callback(GLFWwindow* window, unsigned int codepoint);

void engine_input_key_input_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void engine_input_scroll_input_callback(GLFWwindow* window, double xoffset, double yoffset);

// Mouse button
void engine_input_clear_mouse_button_input();

MouseButtonAction engine_input_get_mouse_button(uint32_t index);

// Cursor position
void engine_input_update_cursor_pos();

const double* engine_input_get_cursor_pos();

// Mouse scroll
void engine_input_clear_mouse_scroll_input();

const double* engine_input_get_mouse_scroll();

// Char input
void engine_input_init_char_buffer();

void engine_input_free_char_buffer();

void engine_input_clear_char_input();

LIST_TYPE(char) engine_input_get_chars_pressed();

// Key input
void engine_input_init_key_buffer();

void engine_input_free_key_buffer();

void engine_input_clear_key_input();

LIST_TYPE(KeyAction) engine_input_get_keys_pressed();