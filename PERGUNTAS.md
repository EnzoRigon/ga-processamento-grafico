# Lista 1 e 2 — OpenGL / GLSL / Pipeline Gráfico

## Lista 1

### 1. O que é a GLSL?
GLSL (OpenGL Shading Language) é a linguagem usada para escrever **shaders** — programas que rodam na GPU.

**Shaders obrigatórios no pipeline programável (OpenGL 3.3+):**
- **Vertex Shader:** processa cada vértice (posições, transformações, atributos).
- **Fragment Shader:** processa cada fragmento (pixel), definindo sua cor final.

---

### 2. O que são primitivas gráficas?
Primitivas são as **formas básicas desenhadas pela GPU**:
- Pontos (`GL_POINTS`)
- Linhas (`GL_LINES`)
- Triângulos (`GL_TRIANGLES`)

**Armazenamento dos vértices:**
- Feito na GPU usando buffers:
  - **VBO:** guarda dados dos vértices.
  - **VAO:** guarda a configuração de como ler o VBO.
  - **EBO (opcional):** guarda índices para reutilizar vértices.

---

### 3. VBO, VAO e EBO

- **VBO (Vertex Buffer Object):** armazena os vértices (posições, cores, etc).
- **VAO (Vertex Array Object):** guarda o estado das ligações entre atributos e buffers.
- **EBO (Element Buffer Object):** armazena índices para evitar duplicação de vértices.

**Relação entre eles:**

    +--------------------+
    |        VAO         |
    |--------------------|
    | VBO -> vértices    |
    | EBO -> índices     |
    +--------------------+


O VAO gerencia, o VBO guarda os vértices e o EBO os índices.

---

## Lista 2

### 3. Utilizando a câmera 2D do exercício anterior, desenhe algo na tela. O que acontece quando posicionamos os objetos? Por que é útil essa configuração?

Ao posicionar objetos no espaço do mundo eles são transformados e, pela câmera 2D, essas coordenadas são mapeadas diretamente para a tela — mover um objeto altera sua posição visual; escalar/rotacionar altera tamanho/orientação.
Essa configuração é útil porque separa geometria de visualização, mantém consistência entre resoluções e simplifica interfaces 2D, tilemaps e múltiplos viewports.