#include "shader.h"

#include "util/util.h"


// Shared
static Shader bind_shader_ = 0;

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

void engine_shader_terminate(Shader shader) {
    glDeleteProgram(shader);
}