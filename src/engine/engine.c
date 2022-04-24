#include "engine.h"

#include "input.h"

// Current dir
static char* current_dir_; // Get the current dir from argv[0]

// Delta time
static const double DELTA_TIME_HIGH = 0.1;

static double delta_time_last_;
static double delta_time_;

// FPS
static double crnt_time_;
static double prev_time_;
static uint32_t fps_counter_;
static uint32_t fps_;

// Performance
void engine_calculate_delta_time() {
    double delta_time_current = glfwGetTime();
    delta_time_ = delta_time_current - delta_time_last_;

    if (delta_time_ >= DELTA_TIME_HIGH) {
        delta_time_ = DELTA_TIME_HIGH;
    }

    delta_time_last_ = delta_time_current;
}

double engine_delta_time() {
    return delta_time_;
}

void engine_calculate_fps() {
    crnt_time_ = glfwGetTime();
    double time_diff = crnt_time_ - prev_time_;
    fps_counter_++;

    if (time_diff >= 1 / 30.0) {
        fps_ = (1 / time_diff) * fps_counter_;

        prev_time_ = crnt_time_;
        fps_counter_ = 0;
    }
}

uint32_t engine_fps() {
    return fps_;
}

// Events
void engine_poll_events() {

    // Clear old events
    engine_input_clear_mouse_button_input();

    engine_input_clear_char_input();

    engine_input_clear_key_input();

    engine_input_clear_mouse_scroll_input();

    // Poll new events
    glfwPollEvents();

    // Update cursor pos
    engine_input_update_cursor_pos();
}