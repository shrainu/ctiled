#include "window.h"

#include "parser/parser.h"

#include "util/map.h"
#include "util/util.h"

#include "input.h"


// Engine properties
static GLFWwindow* window_ = NULL;
static GLFWmonitor* monitor_ = NULL;
static GLFWmonitor** available_monitors_ = NULL;
static int32_t monitor_count_ = 0;

static const char* config_path_ = "config.yaml";

// Window defaults
static const uint32_t SCREEN_WIDTH = 800;
static const uint32_t SCREEN_HEIGHT = 600;
static const bool VSYNC = true;
static const bool RETINA = false;
static const bool MAXIMIZE = false;
static const int32_t WINDOW_MODE = WINDOW_MODE_WINDOWED;
static const uint32_t MONITOR_INDEX = 0;

// Window properties
static const char* title_ = "CTiled";
static uint32_t window_width_ = SCREEN_WIDTH;
static uint32_t window_height_ = SCREEN_HEIGHT;
static bool vsync_ = VSYNC;
static bool retina_ = RETINA;
static bool maximize_ = MAXIMIZE;
static int32_t window_mode_ = WINDOW_MODE;
static uint32_t monitor_index_ = MONITOR_INDEX;

// Static
static void engine_window_parse_properties() {
    Map* properties = parser_parse_yaml(config_path_);
    if (!properties) {
        printf("ERROR: Failed to parse properties from file '%s'.\n", config_path_);
        return;
    }

    // Width
    int width = parser_yaml_parse_int(properties, "width");
    if (width != 0) {
        window_width_ = width;
    }

    // Height
    int height = parser_yaml_parse_int(properties, "height");
    if (height != 0) {
        window_height_ = height;
    }

    // Vsync
    vsync_ = parser_yaml_parse_bool(properties, "vsync");

    // Retina
    retina_ = parser_yaml_parse_bool(properties, "retina");

    // Maximize
    maximize_ = parser_yaml_parse_bool(properties, "maximize");

    // Window mode 
    int window_mode = parser_yaml_parse_int(properties, "window-mode");
    if (window_mode >= 0 && window_mode <= 2) {
        window_mode_ = window_mode;
    }

    // Monitor index
    int monitor_index = parser_yaml_parse_int(properties, "monitor");
    if (monitor_index >= 0) {
        monitor_index_ = monitor_index;
    }

    map_free(properties);
    free(properties);
}

// Init
bool engine_init_window() {
    
    // Initialize GLFW
    if (!glfwInit()) {
        printf("ERROR: GLFW failed to initialize.\n");
        return false;
    }

    // GL window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Window hints
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Hardcoded disabled resizable window feature

    // Parse window properties
    engine_window_parse_properties();

    // Create window
    window_ = glfwCreateWindow(window_width_, window_height_, title_, NULL, NULL);
    if (!(window_)) {
        printf("ERROR: GLFW Window could not be created.\n");
        glfwTerminate();
        return false;
    }

    // Update monitors
    engine_window_update_monitors();

    // Set monitor
    engine_window_set_monitor(monitor_index_);

    // Set the window mode
    engine_window_set_mode(window_mode_);

    // Make window context current
    glfwMakeContextCurrent(window_);

    // Initialize GLEW
    if (glewInit() != 0) { // 0 means success
        printf("ERROR: GLEW failed to initialize.\n");
        glfwTerminate();
        return false;
    }

    // Set VSync
    engine_window_set_vsync(vsync_);

    // Set GL Viewport
    engine_update_gl_viewport();

    // Set callbacks
    glfwSetMouseButtonCallback(window_, engine_input_mouse_button_callback);

    glfwSetCharCallback(window_, engine_input_char_input_callback);

    glfwSetKeyCallback(window_, engine_input_key_input_callback);

    glfwSetScrollCallback(window_, engine_input_scroll_input_callback);

    return true;
}

void engine_terminate_window() {

    glfwTerminate();
}

// Window
GLFWwindow* engine_glfw_window() {
    return window_;
}

void engine_window_update_monitors() {
    available_monitors_ = glfwGetMonitors(&monitor_count_);
}

void engine_window_set_monitor(uint32_t index) {
    monitor_ = available_monitors_[index];
}

void engine_window_set_maximize(bool maximize) {
    maximize_ = maximize;
}

void engine_window_set_vsync(bool vsync) {
    vsync_ = vsync;

    glfwSwapInterval((int) vsync_);
}

void engine_window_set_mode(int32_t window_mode) {

    window_mode_ = window_mode;

    const GLFWvidmode* mode = glfwGetVideoMode(monitor_);

    if (window_mode_ == WINDOW_MODE_WINDOWED) {

        int32_t pos_x = (window_width_  >= mode->width)  ? 0 : (mode->width  - window_width_)  / 2;
        int32_t pos_y = (window_height_ >= mode->height) ? 0 : (mode->height - window_height_) / 2;

        glfwSetWindowMonitor(
            window_, NULL,
            pos_x, pos_y, 
            window_width_, window_height_, 
            GLFW_DONT_CARE
        );

    } else if (window_mode == WINDOW_MODE_FULLSCREEN) {

        if (maximize_) {
            window_width_  = mode->width;
            window_height_ = mode->height;            
        } else {
            if (window_width_ > mode->width) {
                window_width_ = mode->width;
            }
            if (window_height_ > mode->height) {
                window_height_ = mode->height;
            }
        }

        glfwSetWindowMonitor(
            window_, monitor_,
            0, 0, 
            window_width_, window_height_, 
            GLFW_DONT_CARE
        );

    } else if (window_mode_ == WINDOW_MODE_WINDOWED_FULLSCREEN) {
        
        window_width_ = mode->width;
        window_height_ = mode->height;

        glfwSetWindowMonitor(
            window_, NULL,
            0, 0, 
            window_width_, window_height_, 
            GLFW_DONT_CARE
        );
    }
}

void engine_update_gl_viewport() {
    if (retina_) {
        glViewport(0, 0, window_width_ * 2, window_height_ * 2);
    } else {
        glViewport(0, 0, window_width_, window_height_);
    }
}

// Get
vec2s engine_window_get_size() {
    return (vec2s) { .x = window_width_, .y = window_height_ };
}

bool engine_window_get_retina() {
    return retina_;
}