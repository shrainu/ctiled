#include "shader.h"

#include "util/util.h"


// Shared
static Shader bound_shader_ = 0;

// Static
static uint32_t engine_shader_compile(uint32_t type, const char* shader_source) {

	if (shader_source == NULL)
		return 0;

    uint32_t shader = glCreateShader(type);
    const char* src = shader_source;
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {

        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        char* message = (char*) malloc(length * sizeof(char));
        glGetShaderInfoLog(shader, length, &length, message);
		
		char* shader_type = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
		printf("ERROR: '%s' shader failed to compile.\n", shader_type);
		printf("OUTPUT: %s\n", message);
        
        // Cleanup
        glDeleteShader(shader);

        free((void*) shader_source);
        free(message);

        return 0;
    }

    // Cleanup
    free((void*) shader_source);

    return shader;
}

// Shader creation & termination
uint32_t engine_shader_new(const char* vertex_source, const char* fragment_source) {

    uint32_t program = glCreateProgram();
    uint32_t vs = engine_shader_compile(GL_VERTEX_SHADER, read_file(vertex_source));
    uint32_t fs = engine_shader_compile(GL_FRAGMENT_SHADER, read_file(fragment_source));

	if (!(vs) || !(fs)) 
		return 0;

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {

        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
		printf("WARNING: Shader program failed to link.\n");
        printf("OUTPUT: %s\n", infoLog);

        // Cleanup
        glDeleteProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);

        return 0;
    }

    glValidateProgram(program);

    // Cleanup
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void engine_shader_free(Shader shader) {
    glDeleteProgram(shader);
}

// Shader
void engine_shader_bind(Shader shader) {
    glUseProgram(shader);
    bound_shader_ = shader;
}

void engine_shader_unbind(Shader shader) {
    glUseProgram(0);
    bound_shader_ = 0;
}

// Get
Shader engine_bound_shader() {
    return bound_shader_;
}

// Uniforms
void engine_shader_int(Shader shader, const char* location, int32_t value) {
    int32_t loc = glGetUniformLocation(shader, location);

#ifdef DEBUG
    if (loc == -1) {
        printf("WARNING: '%s' is not a valid uniform location.\n", location);
        return;
    }
#endif

    glUniform1i(loc, value);
}

void engine_shader_float(Shader shader, const char* location, float value) {
    int32_t loc = glGetUniformLocation(shader, location);

#ifdef DEBUG
    if (loc == -1) {
        printf("WARNING: '%s' is not a valid uniform location.\n", location);
        return;
    }
#endif

    glUniform1f(loc, value);
}

void engine_shader_vec2(Shader shader, const char* location, vec2 vec2) {
    int32_t loc = glGetUniformLocation(shader, location);

#ifdef DEBUG
    if (loc == -1) {
        printf("WARNING: '%s' is not a valid uniform location.\n", location);
        return;
    }
#endif

    glUniform2f(loc, vec2[0], vec2[1]);
}

void engine_shader_vec3(Shader shader, const char* location, vec3 vec3) {
    int32_t loc = glGetUniformLocation(shader, location);

#ifdef DEBUG
    if (loc == -1) {
        printf("WARNING: '%s' is not a valid uniform location.\n", location);
        return;
    }
#endif

    glUniform3f(loc, vec3[0], vec3[1], vec3[2]);
}

void engine_shader_vec4(Shader shader, const char* location, vec4 vec4) {
    int32_t loc = glGetUniformLocation(shader, location);

#ifdef DEBUG
    if (loc == -1) {
        printf("WARNING: '%s' is not a valid uniform location.\n", location);
        return;
    }
#endif

    glUniform4f(loc, vec4[0], vec4[1], vec4[2], vec4[3]);
}

void engine_shader_mat4(Shader shader, const char* location, mat4 mat4) {
    int32_t loc = glGetUniformLocation(shader, location);

#ifdef DEBUG
    if (loc == -1) {
        printf("WARNING: '%s' is not a valid uniform location.\n", location);
        return;
    }
#endif

    glUniformMatrix4fv(loc, 1, GL_FALSE, mat4[0]);
}