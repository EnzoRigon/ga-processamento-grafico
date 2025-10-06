/*
 * Triângulos com clique do mouse - OpenGL Moderna
 * Cada clique cria um vértice, a cada 3 cliques um triângulo colorido é desenhado.
 * Mapeamento 1:1 com a janela (origem no topo-esquerdo), compatível com HiDPI/Retina.
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Protótipos
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
int setupShader();

// Dimensões iniciais da janela
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const GLchar *vertexShaderSource = R"(
 #version 400
 layout (location = 0) in vec3 position;
 layout (location = 1) in vec4 color;
 uniform mat4 projection;
 out vec4 vColor;
 void main()
 {
     gl_Position = projection * vec4(position, 1.0);
     vColor = color;
 }
)";
const GLchar *fragmentShaderSource = R"(
 #version 400
 in vec4 vColor;
 out vec4 color;
 void main()
 {
     color = vColor;
 }
)";

// Dados globais
std::vector<float> vertices; // x, y, z
std::vector<float> colors;   // r, g, b, a
GLuint VAO = 0, VBO = 0, CBO = 0;
GLuint shaderID = 0;
GLint projLoc = -1;

// Gera cor aleatória
static void randomColor(float &r, float &g, float &b) {
    r = static_cast<float>(rand()) / RAND_MAX;
    g = static_cast<float>(rand()) / RAND_MAX;
    b = static_cast<float>(rand()) / RAND_MAX;
}

// Atualiza a projeção ortográfica usando o tamanho da JANELA (casa com o mouse)
static void updateProjection(GLFWwindow* window) {
    int ww, wh;
    glfwGetWindowSize(window, &ww, &wh); // coordenadas do mouse usam este espaço
    float left = 0.0f, right = static_cast<float>(ww);
    float top = 0.0f, bottom = static_cast<float>(wh);
    float nearp = -1.0f, farp = 1.0f;

    float ortho[16] = {
        2.0f/(right-left), 0, 0, 0,
        0, 2.0f/(top-bottom), 0, 0,
        0, 0, -2.0f/(farp-nearp), 0,
        -(right+left)/(right-left), -(top+bottom)/(top-bottom), -(farp+nearp)/(farp-nearp), 1
    };

    glUseProgram(shaderID);
    if (projLoc < 0) projLoc = glGetUniformLocation(shaderID, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, ortho);
}

// Callback do mouse: cada clique adiciona 1 vértice
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos); // coordenadas da janela (origem topo-esquerdo)

        // Adiciona vértice (z=0)
        vertices.push_back(static_cast<float>(xpos));
        vertices.push_back(static_cast<float>(ypos));
        vertices.push_back(0.0f);

        // Fecha triângulo: gera uma cor e repete para os 3 vértices
        if (vertices.size() % 9 == 0) // 3 vértices * 3 floats
        {
            float r, g, b;
            randomColor(r, g, b);
            for (int i = 0; i < 3; ++i)
            {
                colors.push_back(r);
                colors.push_back(g);
                colors.push_back(b);
                colors.push_back(1.0f);
            }
        }

        // Atualiza buffers (dados dinâmicos)
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

// MAIN
int main()
{
    srand(static_cast<unsigned int>(time(0)));

    // GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Triângulos com Mouse -- Enzo", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Viewport inicial usa FRAMEBUFFER (necessário p/ HiDPI)
    int fbw, fbh;
    glfwGetFramebufferSize(window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);

    // Shaders e projeção
    shaderID = setupShader();
    glUseProgram(shaderID);
    projLoc = glGetUniformLocation(shaderID, "projection");
    updateProjection(window); // projeção baseada no tamanho da janela

    // VAO/VBO/CBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CBO);

    glBindVertexArray(VAO);

    // Vértices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Cores
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);

    // Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Mantém viewport correto (FRAMEBUFFER pode mudar no resize/retina)
        glfwGetFramebufferSize(window, &fbw, &fbh);
        glViewport(0, 0, fbw, fbh);

        // Mantém projeção casada com o tamanho da JANELA (mouse)
        updateProjection(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderID);
        glBindVertexArray(VAO);

        // Desenha apenas triângulos completos
        int nTriangles = static_cast<int>(colors.size()) / (4 * 3);
        glDrawArrays(GL_TRIANGLES, 0, nTriangles * 3);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &CBO);
    glfwTerminate();
    return 0;
}

// Teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Compila e linka shaders
int setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}