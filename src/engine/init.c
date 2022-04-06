#include "init.h"

#include "parser/parser.h"

#include "util/map.h"
#include "util/util.h"


// Engine properties
static GLFWwindow* window_ = NULL;
static GLFWmonitor* monitor_ = NULL;
static uint32_t monitor_count_ = 0;

static const char* config_path_ = "config.yaml";

// Window defaults
static const uint32_t SCREEN_WIDTH = 800;
static const uint32_t SCREEN_HEIGHT = 600;
static const bool VSYNC = true;
static const bool RETINA = false;
static const bool MAXIMIZED = false;
static const int32_t WINDOW_MODE = WINDOW_MODE_WINDOWED;
static const uint32_t MONITOR_INDEX = 0;

// Window properties
static const char* title_ = "BWJam Game";
static uint32_t window_width_ = SCREEN_WIDTH;
static uint32_t window_height_ = SCREEN_HEIGHT;
static bool vsync_ = VSYNC;
static bool retina_ = RETINA;
static bool maximize_ = MAXIMIZED;
static int32_t window_mode_ = WINDOW_MODE;
static uint32_t monitor_index_ = MONITOR_INDEX;

// Static
static void engine_parse_properties() {
    Map* properties = parser_parse_yaml(config_path_);
    if (!properties) {
        printf("ERROR: Failed to parse properties from file '%s'", config_path_);
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

    // Maximized
    maximize_ = parser_yaml_parse_bool(properties, "maximize");

    // Window mode 
    int window_mode = parser_yaml_parse_int(properties, "window-mode");
    if (window_mode >= 0 && window_mode <= 2) {
        window_mode_ = window_mode;
    }
}

// Init
bool engine_init() {
    
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
    engine_parse_properties();

    // Create window
    window_ = glfwCreateWindow(window_width_, window_height_, title_, NULL, NULL);
    if (!(window_)) {
        printf("ERROR: GLFW Window could not be created.\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);

    // Initialize GLEW
    if (glewInit() != 0) { // 0 means success
        printf("ERROR: GLEW failed to initialize.\n");
        glfwTerminate();
        return false;
    }

    // Set VSync
    glfwSwapInterval((int) vsync_);

    if (retina_) {
        glViewport(0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2);
    } else {
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    return true;
}


// Window
GLFWwindow* engine_glfw_window() {
    return window_;
}