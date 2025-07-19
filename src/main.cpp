#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "camera.h"

// Globals
Camera camera;
bool firstMouse = true;
float lastX = 400, lastY = 300;
struct Icon {
    glm::vec3 position;
    float rotation = 0.0f;
    glm::vec3 color;
    GLuint texture;
};
std::vector<Icon> icons;


// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        camera.dragging = (action == GLFW_PRESS);
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    float xf = static_cast<float>(xpos);
    float yf = static_cast<float>(ypos);

    if (camera.dragging) {
        float dx = xf - camera.lastX;
        float dy = yf - camera.lastY;
        camera.handleDrag(dx, dy);
    }

    camera.lastX = xf;
    camera.lastY = yf;
}

std::string loadFile(const char* path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint compileShader(const char* vertexPath, const char* fragmentPath) {
    std::string vSrc = loadFile(vertexPath);
    std::string fSrc = loadFile(fragmentPath);

    const char* vCode = vSrc.c_str();
    const char* fCode = fSrc.c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, NULL);
    glCompileShader(vertex);
    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "Vertex shader error:\n" << infoLog << std::endl;
    }

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, NULL);
    glCompileShader(fragment);
    int success2;
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success2);
    if (!success2) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "Fragment shader error:\n" << infoLog << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return program;
}

GLuint loadTexture(const char* path) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0;
    }

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}

int main() {
    // init GLFW and create window with OpenGL context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(800, 600, "WaraWara - Ground + Drag Camera", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Set up callbacks for window events
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    // Load OpenGL functions using GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.976f, 0.965f, 1.0f, 0.8f); // Pale Blue

    GLuint iconTex = loadTexture("C:\\Users\\tbmh1\\Projects\\Cam U\\warawaraexample\\wara_raw_opengl\\src\\3 (1080 x 1080 px).png");

    // Create floating icons in a circular layout
    int iconCount = 8;
    float radius = 3.0f;
    glm::vec3 colors[] = {
    {1, 0, 0},  // red
    {0, 1, 0},  // green
    {0, 0, 1},  // blue
    {1, 1, 0},  // yellow
    {1, 0, 1},  // magenta
    {0, 1, 1},  // cyan
    {1, 0.5f, 0}, // orange
    {0.5f, 0, 1}  // violet
    };

    for (int i = 0; i < iconCount; ++i) {
        float angle = glm::radians((360.0f / iconCount) * i);
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        icons.push_back({ glm::vec3(x, 1.5f, z), 0.0f, colors[i % 8], iconTex });
    }

    // Create and bind the Vertex Array Object (VAO) and Vertex Buffer Object (VBO) to store the ground plane vertices
    // Ground plane (two triangles)
    float groundVertices[] = {
        -5.0f, 0.0f, -5.0f,
         5.0f, 0.0f, -5.0f,
         5.0f, 0.0f,  5.0f,
        -5.0f, 0.0f, -5.0f,
         5.0f, 0.0f,  5.0f,
        -5.0f, 0.0f,  5.0f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float quadVertices[] = {
        // pos            // tex
        -0.3f,  0.3f, 0.0f,  0.0f, 1.0f,
         0.3f,  0.3f, 0.0f,  1.0f, 1.0f,
         0.3f, -0.3f, 0.0f,  1.0f, 0.0f,
        -0.3f,  0.3f, 0.0f,  0.0f, 1.0f,
         0.3f, -0.3f, 0.0f,  1.0f, 0.0f,
        -0.3f, -0.3f, 0.0f,  0.0f, 0.0f,
    };

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // Compile and link the shader program for vertex and fragment shaders
    GLuint shader = compileShader("C:\\Users\\tbmh1\\Projects\\Cam U\\warawaraexample\\wara_raw_opengl\\src\\shader.vert", "C:\\Users\\tbmh1\\Projects\\Cam U\\warawaraexample\\wara_raw_opengl\\src\\shader.frag");

    // Main game loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = width / (float)height;

        glUseProgram(shader);
        glm::mat4 vp = camera.getViewProj(aspect);
        glUniformMatrix4fv(glGetUniformLocation(shader, "uViewProj"), 1, GL_FALSE, glm::value_ptr(vp));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        float time = glfwGetTime();
        for (Icon& icon : icons) {
            icon.rotation += 50.0f * 0.016f; // rotate over time

            glm::mat4 model = glm::translate(glm::mat4(1.0f), icon.position);
            model = glm::rotate(model, glm::radians(icon.rotation), glm::vec3(0, 1, 0));

            glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, glm::value_ptr(model));

            glBindVertexArray(quadVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, icon.texture);
            glUniform1i(glGetUniformLocation(shader, "uTex"), 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}