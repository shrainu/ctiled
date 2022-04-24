#include "input.h"

#include "window.h"


// Statics
// Cursor pos
static double cursor_pos_[2];

// Mouse scroll
static double mouse_scroll_input_[2];

// Mouse button
static MouseButtonInput mouse_button_input_;

// Char input
static LIST_TYPE(char) char_input_buffer_;

// Key input
static LIST_TYPE(KeyAction) key_input_buffer_;

// Callbacks
void engine_input_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button >= INPUT_MAX_MOUSE_BUTTON) {
        return;
    }

    MouseButtonAction* mba = &mouse_button_input_.buttons[button];

    if (action == GLFW_PRESS) {
        mba->just_pressed = true;
        mba->pressed = true;
    } else if (action == GLFW_RELEASE) {
        mba->just_released = true;
        mba->pressed = false;
    }
}

void engine_input_char_input_callback(GLFWwindow* window, unsigned int codepoint) {

    LIST_PUSH(char_input_buffer_, codepoint);
}

void engine_input_key_input_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    LIST_PUSH(
        key_input_buffer_, 
        ((KeyAction) {
            .key = key,
            .state = action
        })
    );
}

void engine_input_scroll_input_callback(GLFWwindow* window, double xoffset, double yoffset) {

    mouse_scroll_input_[0] = xoffset;
    mouse_scroll_input_[1] = yoffset;
}

// Mouse button
void engine_input_clear_mouse_button_input() {
    for (int32_t i = 0; i < INPUT_MAX_MOUSE_BUTTON; ++i) {
        MouseButtonAction* mba = &mouse_button_input_.buttons[i];

        mba->just_pressed  = false;
        mba->just_released = false;
    }
}

MouseButtonAction engine_input_get_mouse_button(uint32_t index) { // NOTE: Not safe!
    return mouse_button_input_.buttons[index];
}

// Cursor pos
void engine_input_update_cursor_pos() {
    glfwGetCursorPos(engine_glfw_window(), &cursor_pos_[0], &cursor_pos_[1]);
    cursor_pos_[1] = engine_window_get_size().y - cursor_pos_[1];
}

const double* engine_input_get_cursor_pos() {
    return cursor_pos_;
}

// Mouse scroll
void engine_input_clear_mouse_scroll_input() {
    mouse_scroll_input_[0] = 0;
    mouse_scroll_input_[1] = 0;
}

const double* engine_input_get_mouse_scroll() {
    return mouse_scroll_input_;
}

// Char input
void engine_input_init_char_buffer() {
    char_input_buffer_ = LIST_NEW(char_input_buffer_, char);
}

void engine_input_free_char_buffer() {
    LIST_FREE(char_input_buffer_);
}

void engine_input_clear_char_input() {
    LIST_CLEAR(char_input_buffer_);
}

LIST_TYPE(char) engine_input_get_chars_pressed() {
    return char_input_buffer_;
}

// Key input
void engine_input_init_key_buffer() {
    key_input_buffer_ = LIST_NEW(key_input_buffer_, KeyAction);
}

void engine_input_free_key_buffer() {
    LIST_FREE(key_input_buffer_);
}

void engine_input_clear_key_input() {
    LIST_CLEAR(key_input_buffer_);
}

LIST_TYPE(KeyAction) engine_input_get_keys_pressed() {
    return key_input_buffer_;
}