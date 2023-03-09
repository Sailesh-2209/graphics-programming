#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


enum ShaderType { V_SHADER, F_SHADER } SHADER_TYPE;


struct Shaders {
    std::string vertexShader;
    std::string fragmentShader;
} shaders;


static unsigned int CompileShader(unsigned int type, const char* src) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}


static void readShaderSource(const char* shaderFile) {
    std::ifstream file(shaderFile);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string buffString = buffer.str();
    if (SHADER_TYPE == V_SHADER) {
        shaders.vertexShader = buffString;
    }
    else if (SHADER_TYPE == F_SHADER) {
        shaders.fragmentShader = buffString;
    }
    else {
        return;
    }
}


static unsigned int CreateShader(const char* vertexShaderFile, const char* fragmentShaderFile) {
    unsigned int program = glCreateProgram();

    SHADER_TYPE = V_SHADER;
    readShaderSource(vertexShaderFile);
    SHADER_TYPE = F_SHADER;
    readShaderSource(fragmentShaderFile);

    const char* vertexShaderSource = shaders.vertexShader.c_str();
    const char* fragmentShaderSource = shaders.fragmentShader.c_str();

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


int main(void) {
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(16 * 100, 9 * 100, "OpenGL Window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    unsigned int err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;
    
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    const char* vertexShaderFile = "res/shaders/vshader.glsl";
    const char* fragmentShaderFile = "res/shaders/fshader.glsl";

    unsigned int shader = CreateShader(vertexShaderFile, fragmentShaderFile);
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
