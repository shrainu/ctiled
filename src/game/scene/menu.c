#include "menu.h"

#include "game/ui/ui.h"
#include "game/ui/label.h"
#include "game/ui/button.h"
#include "game/ui/input.h"
#include "game/ui/panel.h"

#include "engine/engine.h"
#include "engine/input.h"
#include "engine/window.h"
#include "engine/renderer.h"
#include "engine/font.h"
#include "engine/texture.h"

#include "util/list.h"


// Add scene definitions
SCENE_DEFINE(menu);

// Defines
#define INITIAL_LEVEL_SIZE  512
#define TILE_SIZE           32

// Static elements
static UINode* panel;
static UINode* level_path_node;
static UINode* tileset_node;
static UINode* tileset_width_node;
static UINode* tileset_height_node;

// State
static bool can_place_tiles_ = true;
static bool show_exit_panel_ = false;

// Resources
static Font* default_font_;

// Level
static int32_t* level_data_;

// Camera
typedef struct Camera {
    vec2s position;
} Camera;

static Camera camera_;
static double camera_speed_ = 200.0f;

// Tile
typedef struct Tile {
    vec2s pos;
    vec4s source;
    float size;
    uint32_t index;
} Tile;
LIST_DECLARE(Tile);

// Tilepicker
typedef struct Tilepicker {
    vec3s pos;
    vec2s size;
    float max_scroll;
    float render_offset;
    Texture* tileset;

    bool cursor_over;
    bool show_tileset;

    uint32_t tile_width;
    uint32_t tile_height;

    float total_height;
    int32_t max_index;
    LIST_TYPE(Tile) tiles;
    int32_t selected_tile;
} Tilepicker;

static Tilepicker* tilepicker_;

// Functionality
void exit_app() {
    glfwSetWindowShouldClose(engine_glfw_window(), true);
}

void close_exit_panel() {
    show_exit_panel_ = false;
    can_place_tiles_ = true;
}

void save_map() {
    UIInput* level_path_input = ui_input_get(level_path_node);
    const char* path = level_path_input->buffer->array;

    printf("INFO: Saving level to '%s'.\n", path);

    if (level_path_input->buffer->count == 0) {
        printf("ERROR: Filename can't be empty.\n");
        return;
    }

    FILE* file = fopen(path, "w");
    int32_t written = fwrite(level_data_, sizeof(int32_t), (INITIAL_LEVEL_SIZE * INITIAL_LEVEL_SIZE), file);

    printf("INFO: Level has been saved, written %.2f MB of memory.\n", ((double)written * 4) / pow(2, 20));
}

void load_map() {
    UIInput* level_path_input = ui_input_get(level_path_node);
    const char* path = level_path_input->buffer->array;

    printf("INFO: Loading level from '%s'.\n", path);

    FILE* file;
    if (!(file = fopen(path, "r"))) {
        printf("ERROR: File '%s' could not be opened.\n", path);
        return;
    }

    uint32_t read = fread(level_data_, sizeof(int32_t), (INITIAL_LEVEL_SIZE * INITIAL_LEVEL_SIZE), file);

    printf("INFO: Level has been loaded, read %.2f MB of memory.\n", ((double)read * 4) / pow(2, 20));
}

void clear_map() {
    printf("INFO: Clearing the level.\n");

    size_t level_size = (INITIAL_LEVEL_SIZE * INITIAL_LEVEL_SIZE);
    for (int i = 0; i< level_size; ++i) {
        level_data_[i] = -1;
    }
}

void reload_tilepicker() {

    UIInput* tileset_input = ui_input_get(tileset_node);
    FILE* path = fopen(tileset_input->buffer->array, "r");
    if (!path) {
        printf("ERROR: File could not be located to load tileset.\n");

        // If tileset could not be loaded don't show it
        tilepicker_->show_tileset = false;

        return;
    } 

    // Reload the tileset
    if (tilepicker_->tileset) {
        engine_texture_free(tilepicker_->tileset); 
    }

    tilepicker_->tileset = engine_texture_new(tileset_input->buffer->array, GL_NEAREST);

    // Reload the size
    UIInput* tileset_width_input  = ui_input_get(tileset_width_node);
    UIInput* tileset_height_input = ui_input_get(tileset_height_node);

    tilepicker_->tile_width  = atoi(tileset_width_input->buffer->array);
    tilepicker_->tile_height = atoi(tileset_height_input->buffer->array);

    // If width or height is 0 don't show tileset
    if (!(tilepicker_->tile_width && tilepicker_->tile_height)) {
        printf("ERROR: Tile width and/or height could not be read.\n");
        tilepicker_->show_tileset = false;
        return;
    }

    // Recreate tiles
    LIST_CLEAR(tilepicker_->tiles);

    uint32_t tile_in_row = tilepicker_->tileset->width  / tilepicker_->tile_width;
    uint32_t tile_in_col = tilepicker_->tileset->height / tilepicker_->tile_height;
    uint32_t tile_count = tile_in_row * tile_in_col;

    float render_size = tilepicker_->size.x / 3.0f;

    tilepicker_->total_height = ((tile_count / 3) * render_size) + render_size;
    tilepicker_->max_scroll = tilepicker_->total_height - tilepicker_->size.y;
    tilepicker_->max_index = tile_count - 1;

    for (int32_t i = 0; i < tile_count; ++i) {

        Tile tile = (Tile) {
            .pos = (vec2s) {
                tilepicker_->pos.x + ((i % 3) * render_size),
                tilepicker_->pos.y + tilepicker_->size.y - ((i / 3) * render_size) - render_size
            },
            .source = (vec4s) {
                ((i % tile_in_row) * tilepicker_->tile_width) / (float) tilepicker_->tileset->width, 
                (tilepicker_->tileset->height - (((i / tile_in_row) * tilepicker_->tile_height) + tilepicker_->tile_height)) / (float) tilepicker_->tileset->height,
                tilepicker_->tile_width  / (float) tilepicker_->tileset->width, 
                tilepicker_->tile_height / (float) tilepicker_->tileset->height
            },
            .size = render_size,
            .index = i
        };

        LIST_PUSH(tilepicker_->tiles, tile);
    }

    // Show tileset
    tilepicker_->show_tileset = true;
}

void render_tilepicker(Shader shader) {

    // Draw the tilepicker background
    engine_shader_bind(shader);
    engine_shader_vec4(shader, "u_color", (vec4) {0.3, 0.3, 0.3, 1.0});

    engine_render_quad(NULL, NULL, tilepicker_->pos.raw, tilepicker_->size.raw);

    engine_shader_unbind(shader);

    // Enable scissor mode
    engine_renderer_set_scissor(true);
    engine_renderer_set_scissor_box(tilepicker_->pos.x, tilepicker_->pos.y, tilepicker_->size.x, tilepicker_->size.y);

    // Draw the tiles
    if (tilepicker_->show_tileset) {

        vec2s render_size = {
            LIST_GET(tilepicker_->tiles, 0).size,
            LIST_GET(tilepicker_->tiles, 0).size
        };
        
        for (int32_t i = 0; i < tilepicker_->tiles->count; ++i) {

            Tile* current = &LIST_GET(tilepicker_->tiles, i);

            vec3 render_pos = {
                current->pos.x,
                tilepicker_->render_offset + current->pos.y,
                -1.0f
            };

            engine_shader_bind(shader);
            engine_shader_vec4(shader, "u_color", (vec4) {1.0, 1.0, 1.0, 1.0});

            engine_render_quad(tilepicker_->tileset, current->source.raw,  render_pos, render_size.raw);

            engine_shader_unbind(shader);
        }
    }

    // Disable scissor mode
    engine_renderer_set_scissor(false);
}

void update_tilepicker(const double* scroll_input, const double* cursor_pos) {

    // Mouse button
    MouseButtonAction action = engine_input_get_mouse_button(0);

    // Scroll
    if ((cursor_pos[0] >= tilepicker_->pos.x && cursor_pos[0] <= tilepicker_->pos.x + tilepicker_->size.x) &&
        (cursor_pos[1] >= tilepicker_->pos.y && cursor_pos[1] <= tilepicker_->pos.y + tilepicker_->size.y)) {
        tilepicker_->cursor_over = true;
    } else {
        tilepicker_->cursor_over = false;
    }

    if (tilepicker_->cursor_over) {
        tilepicker_->render_offset -= scroll_input[1];

        if (tilepicker_->render_offset < 0) {
            tilepicker_->render_offset = 0;
        }

        if (tilepicker_->max_scroll <= tilepicker_->size.y) {
            tilepicker_->render_offset = 0;
        } else if (tilepicker_->render_offset > tilepicker_->max_scroll) {
            tilepicker_->render_offset = tilepicker_->max_scroll;
        }
    }

    // Select tiles
    bool selected = false;
    for (int32_t i = 0; i < tilepicker_->tiles->count; ++i) {

        Tile* current = &LIST_GET(tilepicker_->tiles, i);

        vec2s current_pos = (vec2s) {
            current->pos.x,
            tilepicker_->render_offset + current->pos.y,
        };

        if ((cursor_pos[0] >= current_pos.x && cursor_pos[0] <= current_pos.x + current->size) &&
            (cursor_pos[1] >= current_pos.y && cursor_pos[1] <= current_pos.y + current->size)) {
        
            if (action.just_pressed) {
                tilepicker_->selected_tile = current->index;
                selected = true;
            }
        }

        if (selected) {
            break;
        }
    }
}

void place_tiles(const double* cursor_pos, vec2s win_size) {
    // Check if cursor is over UI
    if ((cursor_pos[0] >= panel->pos.x && cursor_pos[0] <= panel->pos.x + panel->size.x) &&
        (cursor_pos[1] >= panel->pos.y && cursor_pos[1] <= panel->pos.y + panel->size.y)) {
        return;
    }

    // Mouse button
    MouseButtonAction action_place  = engine_input_get_mouse_button(0);
    MouseButtonAction action_remove = engine_input_get_mouse_button(1);

    // If a tileset isn't bound skip
    if (!tilepicker_->show_tileset || !can_place_tiles_) {
        return;
    }

    // Place tiles
    bool place  = false;
    bool remove = false;

    if (action_place.just_pressed | action_place.pressed) {
        place = true;
    } else if (action_remove.just_pressed | action_remove.pressed) {
        remove = true;
    }

    int32_t x = ((int)cursor_pos[0] + camera_.position.x) / TILE_SIZE;
    int32_t y = ((int)cursor_pos[1] + camera_.position.y) / TILE_SIZE;

    if ((x < 0 || x >= INITIAL_LEVEL_SIZE) || (y < 0 || y >= INITIAL_LEVEL_SIZE)) {
        return;
    }

    if (place) {
        level_data_[(y * INITIAL_LEVEL_SIZE) + x] = tilepicker_->selected_tile;
    } else if (remove) {
        level_data_[(y * INITIAL_LEVEL_SIZE) + x] = -1;
    }
}

void render_tiles(Shader shader) {
    
    bool debug_draw = false;
    if (!tilepicker_->tileset || !tilepicker_->show_tileset) {
        debug_draw = true;
    }

    vec3s render_pos = (vec3s) {
        0 - camera_.position.x, 
        0 - camera_.position.y, 
        -1.0
    };

    vec2s render_size = (vec2s) {
        TILE_SIZE,
        TILE_SIZE
    };

    for (int32_t y = 0; y < INITIAL_LEVEL_SIZE; ++y) {

        render_pos.x = 0 - camera_.position.x;

        for (int32_t x = 0; x < INITIAL_LEVEL_SIZE; ++x) {

            int32_t current = level_data_[(y * INITIAL_LEVEL_SIZE) + x];
            if (current == -1) {
                render_pos.x += TILE_SIZE;

                continue;
            }

            engine_shader_bind(shader);
 
            if (debug_draw || current > tilepicker_->max_index) {
                engine_shader_vec4(shader, "u_color", (vec4) {1.0, 0.0, 1.0, 1.0});

                engine_render_quad(
                    NULL, 
                    NULL, 
                    render_pos.raw, 
                    render_size.raw
                );
            } else {
                engine_shader_vec4(shader, "u_color", (vec4) {1.0, 1.0, 1.0, 1.0});

                engine_render_quad(
                    tilepicker_->tileset, 
                    LIST_GET(tilepicker_->tiles, current).source.raw, 
                    render_pos.raw, 
                    render_size.raw
                );
            }

            engine_shader_unbind(shader);

            render_pos.x += TILE_SIZE;
        }

        render_pos.y += TILE_SIZE;
    }
}

void update_camera(double delta_time) {

    GLFWwindow* window = engine_glfw_window();

    if (glfwGetKey(window, GLFW_KEY_A)) {
        camera_.position.x -= camera_speed_ * delta_time;
    } else if (glfwGetKey(window, GLFW_KEY_D)) {
        camera_.position.x += camera_speed_ * delta_time;
    }
    
    if (glfwGetKey(window, GLFW_KEY_S)) {
        camera_.position.y -= camera_speed_ * delta_time;
    } else if (glfwGetKey(window, GLFW_KEY_W)) {
        camera_.position.y += camera_speed_ * delta_time;
    }

    if (camera_.position.x < 0) {
        camera_.position.x = 0;
    }
    if (camera_.position.y < 0) {
        camera_.position.y = 0;
    }
}

// Load
void game_scene_menu_load() {
    scene_id_ = game_scene_get_new_id();

    uint32_t font_size = (engine_window_get_retina()) ? FONT_DEFAULT_PIXEL_SIZE : 12;

    default_font_ = engine_font_new(
        "res/font/FiraMono-Regular.ttf", 
        font_size, 
        GL_LINEAR
    );
}

// Free
void game_scene_menu_free() {

    engine_font_free(default_font_);
}

// Main
int32_t game_scene_menu() {

    // Get the active scene
    uint32_t active_scene = game_scene_get_active_scene();

    if (active_scene != scene_id_) {
        return SCENE_SKIPPED;
    }

    // Setup
    GLFWwindow* window = engine_glfw_window();
    vec2s win_size = engine_window_get_size();

    // FPS
    char fps_buffer[32];
    double fps_timer = 3.0;
    vec2s fps_size = engine_font_get_text_size(default_font_, "0000", (engine_window_get_retina()) ? 0.25 : 1.0);

    // Level
    size_t level_size = INITIAL_LEVEL_SIZE * INITIAL_LEVEL_SIZE;
    level_data_ = (int32_t*) malloc(sizeof(int32_t) * level_size);
    for (uint32_t i = 0; i < level_size; ++i) {
        level_data_[i] = -1;
    }

    // Camera
    camera_ = (Camera) {
        .position = (vec2s) {0, 0}
    };

    // UI
    ui_init();

    vec2s pos;

    panel = ui_panel_new("Settings", (vec2s) {win_size.x - 250, 0}, (vec2s) {250, win_size.y});

    UINode* level_label = ui_label_new("Level", (vec2s) {50, 50});
    ui_panel_add_node(panel, level_label);

    pos = level_label->rel_pos;
    ui_label_set_alignment_to_parent(level_label, 0);
    ui_node_set_position(level_label, (vec2s) {level_label->rel_pos.x, pos.y});
    
    UINode* filename = ui_label_new("Filepath:", (vec2s) {0, 0});
    ui_panel_add_node(panel, filename);

    level_path_node = ui_input_new("example.level", (vec2s) {0, 0});
    ui_panel_add_node(panel, level_path_node);
    level_path_node->size.x = 250 - (filename->rel_pos.x * 2);

    UINode* save_button = ui_button_new("Save", (vec2s) {0, 0}, save_map);
    ui_panel_add_node(panel, save_button);

    UINode* load_button = ui_button_new("Load", (vec2s) {0, 0}, load_map);
    ui_panel_add_node(panel, load_button);

    UINode* clear_button = ui_button_new("Clear", (vec2s) {0, 0}, clear_map);
    ui_panel_add_node(panel, clear_button);

    UINode* tileset_label = ui_label_new("Tileset", (vec2s) {0, 0});
    ui_panel_add_node(panel, tileset_label);
    
    pos = tileset_label->rel_pos;
    ui_label_set_alignment_to_parent(tileset_label, 0);
    ui_node_set_position(tileset_label, (vec2s) {tileset_label->rel_pos.x, pos.y});

    UINode* tileset_file_label = ui_label_new("Filepath:", (vec2s) {0, 0});
    ui_panel_add_node(panel, tileset_file_label);

    tileset_node = ui_input_new("tileset.png", (vec2s) {0, 0});
    ui_panel_add_node(panel, tileset_node);
    tileset_node->size.x = 250 - (filename->rel_pos.x * 2);

    UINode* tile_width_label = ui_label_new("Tile width:", (vec2s) {0, 0});
    ui_panel_add_node(panel, tile_width_label);

    tileset_width_node = ui_input_new("Width", (vec2s) {0, 0});
    ui_panel_add_node(panel, tileset_width_node);

    UINode* tile_height_label = ui_label_new("Tile height:", (vec2s) {0, 0});
    ui_panel_add_node(panel, tile_height_label);

    tileset_height_node = ui_input_new("Height", (vec2s) {0, 0});
    ui_panel_add_node(panel, tileset_height_node);

    UINode* tileset_reload_button = ui_button_new("Reload", (vec2s) {0, 0}, reload_tilepicker);
    ui_panel_add_node(panel, tileset_reload_button);

    // Exit panel
    UINode* exit_panel = ui_panel_new(
        "Exit the app?",
        (vec2s) {
            (win_size.x - 182.5) / 2,
            (win_size.y - 64) / 2,
        },
        (vec2s)  {182.5, 64}
    );

    UINode* exit_button = ui_button_new("Exit", (vec2s) {0, 0}, exit_app);
    ui_panel_add_node(exit_panel, exit_button);

    UINode* exit_cancel_button = ui_button_new("Cancel", (vec2s) {0, 0}, close_exit_panel);
    ui_panel_add_node(exit_panel, exit_cancel_button);

    ui_node_set_position(exit_cancel_button, (vec2s) {exit_button->rel_pos.x + exit_button->size.x + 7.5, exit_button->rel_pos.y});

    // New Tilepicker
    tilepicker_ = (Tilepicker*) malloc(sizeof(Tilepicker));
    *tilepicker_ = (Tilepicker) {
        .pos  = {
            win_size.x - 250 + filename->rel_pos.x, 
            20,
            -1.0
        },
        .size = {
            250 - (filename->rel_pos.x * 2),
            win_size.y - ((UIPanel*)panel->element)->element_offset[1] - 27.5
        },
        .max_scroll = 0,
        .render_offset = 0,
        .tileset = NULL,
        .show_tileset = false,

        .tile_width = 0,
        .tile_height = 0,

        .max_index = -1,
        .total_height = 0,
        .selected_tile = -1,
    };
    tilepicker_->tiles = LIST_NEW(tilepicker_->tiles, Tile);

    // Loop
    while(active_scene == scene_id_ && !glfwWindowShouldClose(window)) {
        
        // Pre update calculations
        engine_calculate_delta_time();
        engine_calculate_fps();

        double delta_time = engine_delta_time();
        uint32_t fps = engine_fps();

        fps_timer += delta_time;
        if (fps_timer >= 0.5) {
            sprintf(fps_buffer, "%u", fps);
            fps_timer = 0.0;
        }

        LIST_TYPE(KeyAction) keys_pressed = engine_input_get_keys_pressed();
        const double* scroll_input = engine_input_get_mouse_scroll();
        const double* cursor_pos   = engine_input_get_cursor_pos();

        Shader quad_shader = engine_renderer_quad_shader();

        // EVENT
        for (int32_t i = 0; i < keys_pressed->count; ++i) {
            KeyAction key = LIST_GET(keys_pressed, 0);

            if (key.key == GLFW_KEY_ESCAPE && key.state == INPUT_KEY_PRESS) {
                if (!show_exit_panel_) {
                    show_exit_panel_ = true;
                    can_place_tiles_ = false;
                } else {
                    close_exit_panel();
                }
            }
        }

        // UPDATE
        update_tilepicker(scroll_input, cursor_pos);

        update_camera(delta_time);

        place_tiles(cursor_pos, win_size);

        // RENDER
        glClearColor(0.06, 0.05, 0.11, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render tiles
        render_tiles(quad_shader);

        // Update the panel
        ui_panel_update(panel);

        // Draw the tilepicker
        render_tilepicker(quad_shader);

        // Draw the exit panel
        if (show_exit_panel_) {
            ui_panel_update(exit_panel);
        }

        // Render fps
        engine_render_text(
            default_font_, 
            (vec3){5, win_size.y - 5 - fps_size.y, -1.0}, 
            fps_buffer, COLOR_WHITE, (engine_window_get_retina()) ? 0.25 : 1.0
        );

        glfwSwapBuffers(window);
        engine_poll_events();

        // Check the active scene
        active_scene = game_scene_get_active_scene();
    }

    // Testing
    ui_free();

    ui_node_free(panel);

    // Free tilepicker
    if (tilepicker_->tileset) {
        engine_texture_free(tilepicker_->tileset);
    }
    LIST_FREE(tilepicker_->tiles);
    free(tilepicker_);

    // Free level
    free(level_data_);

    return SCENE_EXECUTED;
}