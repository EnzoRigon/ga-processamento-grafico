/*
 * Hello Casa - OpenGL Moderna
 * Desenha uma casa simples (base, telhado e porta)
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int setupGeometry();

// Dimensões da janela
const GLuint WIDTH = 800, HEIGHT = 600;

// Vertex Shader: recebe posição e cor
const GLchar *vertexShaderSource = R"(
 #version 400
 layout (location = 0) in vec3 position;
 layout (location = 1) in vec3 color;
 out vec3 fragColor;
 void main()
 {
     gl_Position = vec4(position, 1.0);
     fragColor = color;
 }
 )";

// Fragment Shader: usa cor do atributo
const GLchar *fragmentShaderSource = R"(
 #version 400
 in vec3 fragColor;
 out vec4 color;
 void main()
 {
     color = vec4(fragColor, 1.0);
 }
 )";

// Função MAIN
int main()
{
    // Inicialização da GLFW
    glfwInit();

    // Configuração do contexto OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Criação da janela GLFW
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Casa OpenGL -- Enzo", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Callback de teclado
    glfwSetKeyCallback(window, key_callback);

    // GLAD: carrega funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Informações de versão
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compila shaders e cria programa
    GLuint shaderID = setupShader();

    // Cria VAO/VBO/EBO com a casa
    GLuint VAO = setupGeometry();

    glUseProgram(shaderID);

    // Loop principal
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // céu azul claro
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        // Base da casa (quadrado marrom)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Telhado (triângulo vermelho)
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));

        // Porta (quadrado bege)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(9 * sizeof(GLuint)));

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

// Callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Compila shaders e retorna o programa
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
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Cria VAO/VBO/EBO com a casa (base, telhado, porta)
int setupGeometry()
{
    // Base (marrom), Telhado (vermelho), Porta (bege)
    GLfloat vertices[] = {
        // Base da casa (marrom)
        -0.4f, -0.5f, 0.0f,   0.55f, 0.27f, 0.07f, // 0: inferior esquerdo
         0.4f, -0.5f, 0.0f,   0.55f, 0.27f, 0.07f, // 1: inferior direito
         0.4f,  0.1f, 0.0f,   0.55f, 0.27f, 0.07f, // 2: superior direito
        -0.4f,  0.1f, 0.0f,   0.55f, 0.27f, 0.07f, // 3: superior esquerdo

        // Telhado (vermelho)
        -0.5f,  0.1f, 0.0f,   0.8f, 0.0f, 0.0f,    // 4: esquerda
         0.5f,  0.1f, 0.0f,   0.8f, 0.0f, 0.0f,    // 5: direita
         0.0f,  0.5f, 0.0f,   0.8f, 0.0f, 0.0f,    // 6: topo

        // Porta (bege)
        -0.08f, -0.5f, 0.0f,  0.96f, 0.87f, 0.70f, // 7: inferior esquerdo
         0.08f, -0.5f, 0.0f,  0.96f, 0.87f, 0.70f, // 8: inferior direito
         0.08f, -0.1f, 0.0f,  0.96f, 0.87f, 0.70f, // 9: superior direito
        -0.08f, -0.1f, 0.0f,  0.96f, 0.87f, 0.70f  //10: superior esquerdo
    };

    GLuint indices[] = {
        // Base (quadrado)
        0, 1, 2,
        2, 3, 0,
        // Telhado (triângulo)
        4, 5, 6,
        // Porta (quadrado)
        7, 8, 9,
        9,10, 7
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);
    // cor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}