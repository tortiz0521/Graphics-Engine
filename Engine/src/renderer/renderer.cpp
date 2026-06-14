#include "renderer.h"
#include <iostream>

void Renderer::InitRenderer(unsigned int WIDTH, unsigned int HEIGHT)
{
    CreateContext(WIDTH, HEIGHT);
    // Initiazlize dynamic buffer
    glGenBuffers(1, &this->m_DynamicVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_DynamicVBO);
    glBufferData(GL_ARRAY_BUFFER, 100.0f * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

    // Initialize UBOs
    //m_dirUBO = std::make_unique<UniformBuffer<DirectionLight>>(UniformBuffer<DirectionLight>(5, 0));
    //m_pointUBO = std::make_unique<UniformBuffer<PointLight>>(UniformBuffer<PointLight>(10, 1));
    //m_spotUBO = std::make_unique<UniformBuffer<SpotLight>>(UniformBuffer<SpotLight>(10, 2));
    //m_camUBO = std::make_unique<UniformBuffer<CameraData>>(UniformBuffer<CameraData>(1, 3));
}

void Renderer::BindUBOs(const std::vector<uint32_t>& IDs)
{
    for (uint32_t ID : IDs) {
        glUseProgram(ID);
        m_dirUBO->Bind(glGetUniformBlockIndex(ID, "DirectionLights"));
        m_pointUBO->Bind(glGetUniformBlockIndex(ID, "PointLights"));
        m_spotUBO->Bind(glGetUniformBlockIndex(ID, "SpotLights"));
        m_camUBO->Bind(glGetUniformBlockIndex(ID, "CameraData"));
    }
}

void Renderer::ShutDownRenderer()
{
    glDeleteBuffers(1, &m_DynamicVBO);

    m_dirUBO->DeleteUBO();
    m_camUBO->DeleteUBO();
    m_pointUBO->DeleteUBO();
    m_spotUBO->DeleteUBO();
}

bool Renderer::CreateContext(unsigned int WIDTH, unsigned int HEIGHT)
{
#ifdef __EMSCRIPTEN__
    setupWindow();
    emscripten_webgl_make_context_current(ctx);
    emscripten_set_canvas_element_size("#canvas", WIDTH, HEIGHT);
	glViewport(0, 0, WIDTH, HEIGHT);
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, key_callback);
    //emscripten_set_mousedown_callback("#canvas", nullptr, true, mouse_callback);
    emscripten_set_mousemove_callback("#canvas", nullptr, true, mouse_callback);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, resize_callback);
#else
    GLFWwindow* current;
    if (!setupWindow(WIDTH, HEIGHT, m_window)) {
        std::cout << "ERROR::WINDOW_FAILED_TO_LOAD\n";
        glfwTerminate();
        return -1;
    }

    current = glfwGetCurrentContext();
    if (current != m_window.get())
        std::cout << "HERE";

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity,
            GLsizei length, const GLchar* message, const void* userParam)
        {
            std::cerr << "OpenGL DEBUG: " << message << std::endl;
        },
        nullptr);

    // Setup the viewport!
    glViewport(0, 0, WIDTH, HEIGHT);

    // Set callbacks:
    glfwSetKeyCallback(m_window.get(), key_callback);
    glfwSetFramebufferSizeCallback(m_window.get(), framebuffer_size_callback);
    glfwSetCursorPosCallback(m_window.get(), mouse_callback);

    // Tell the window to hide the cursor and 'capture' it to the screen.
    glfwSetInputMode(m_window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
    // Tell OpenGL to use the z-buffer for depth tests!
    //glEnable(GL_DEPTH_TEST);
    return true;
}


void Renderer::DrawInstanced(std::shared_ptr<LoadedModel> model, std::shared_ptr<Shader> shader, const std::vector<glm::mat4>& data)
{
    float current =
#ifdef __EMSCRIPTEN__
    emscripten_get_now() * 0.001;
#else
    glfwGetTime();
#endif
    dt = current - last;
    last = current;

#ifdef __EMSCRIPTEN__
    processInput();
#else
    glfwPollEvents();
    processInput(m_window.get());
#endif

    shader->Use();
    UpdateVBO(data);
    //_CrtCheckMemory();

    for (const Mesh& mesh : model->_meshes) {
       mesh.Draw(*shader.get(), data.size());
    }
}

void Renderer::Draw(const LoadedModel &m, const Shader &s, const glm::vec3 &position,
        const glm::vec3 &size,const glm::vec3 color)
{
    s.Use();

    glm::mat4 model = glm::mat4(1.0f);

    // Translate
    model = glm::translate(model, position);

    // Rotate
#ifdef __EMSCRIPTEN__
    model = glm::rotate(model, glm::radians((float)(emscripten_get_now() * 0.001) * 15.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
#else
    model = glm::rotate(model, glm::radians((float)glfwGetTime() * 15.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
#endif

    // Scale
    model = glm::scale(model, size);

    s.SetMatrix4("model", model, true);

    glActiveTexture(GL_TEXTURE0);
    for (const Mesh& mesh : m._meshes) {
        mesh.Draw(s);
    }
}

void Renderer::UpdateVBO(const std::vector<glm::mat4>& data)
{
    // A Non-persistent Map Orphaning implementation
    size_t dataSize = data.size() * sizeof(glm::mat4);
    glBindBuffer(GL_ARRAY_BUFFER, m_DynamicVBO);

    GLint bufferSize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

    if (dataSize > bufferSize) {
        std::cout << "HEAP_ALLOCATION_ERROR::WRITING_PAST_ALLOCATED_MEMORY_SIZE" << std::endl;
        //__debugbreak();
    }

    // Be careful here! Flickering can happen if the GL_MAP_UNSYCHRONIZED_BIT flag is set. Causes a race consdition between the CPU
    // and GPU. CPU gets back to here before the GPU is actually finished rendering.
    auto* ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, dataSize,
        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    memcpy(ptr, data.data(), dataSize);
    if (glUnmapBuffer(GL_ARRAY_BUFFER) == GL_FALSE) {
        std::cout << "BUFFER_CORRUPTION";
    }
}

void Renderer::UpdateLightUBO(const LightVariant& light, size_t index)
{
    switch (light.index()) {
        case 0: 
            if (index >= m_dirUBO->size())
                m_dirUBO->AddStruct(std::get<DirectionLight>(light));
            else
                m_dirUBO->UpdateStruct(std::get<DirectionLight>(light), index);
            break;
        case 1: 
            if (index >= m_pointUBO->size())
                m_pointUBO->AddStruct(std::get<PointLight>(light));
            else    
                m_pointUBO->UpdateStruct(std::get<PointLight>(light), index);
            break;
        case 2: 
            if (index >= m_spotUBO->size())
                m_spotUBO->AddStruct(std::get<SpotLight>(light));
            else
                m_spotUBO->UpdateStruct(std::get<SpotLight>(light), index);
            break;
        default: std::cout << "ENCOUNTERED AN ISSUE IN RENDERER::UPDATE_LIGHT_UBO" << std::endl;
    }
}

void Renderer::UpdateCamUBO(const glm::mat4& camView)
{
    m_camUBO->UpdateField(sizeof(glm::mat4), sizeof(glm::mat4), &camView, 0);
}

void Renderer::UpdateCamProj(const glm::mat4& proj)
{
    m_camUBO->UpdateField(0, sizeof(glm::mat4), &proj, 0);
}

void Renderer::AddUBOStruct(const LightVariant& light)
{
    switch (light.index()) {
        case 0: 
            m_dirUBO->AddStruct(std::get<DirectionLight>(light));
            break;
        case 1: 
            m_pointUBO->AddStruct(std::get<PointLight>(light));
            break;
        case 2: 
            m_spotUBO->AddStruct(std::get<SpotLight>(light));
            break;
        default: std::cout << "ENCOUNTERED AN ISSUE IN RENDERER::ADD_UBO_STRUCT" << std::endl;
    }
}

// NO RESET ON unique_ptr! 'operator=' on a unique pointer will automatically perform a 'reset' of sorts, calling the deleter
// on whatever old memory was there and assigning your unique pointer lvalue to the rvalue reference passed int
void Renderer::ClearUBOs()
{
    m_dirUBO = std::make_unique<UniformBuffer<DirectionLight>>(UniformBuffer<DirectionLight>(5, 0));
    m_pointUBO = std::make_unique<UniformBuffer<PointLight>>(UniformBuffer<PointLight>(10, 1));
    m_spotUBO = std::make_unique<UniformBuffer<SpotLight>>(UniformBuffer<SpotLight>(10, 2));
}


void Renderer::SwapBuffers()
{
#ifndef __EMSCRIPTEN__
    glfwSwapBuffers(m_window.get());
#endif
}

unsigned int Renderer::GetDynamicVBO()
{
    return m_DynamicVBO;
}

void Renderer::ProcessInput()
{
#ifdef __EMSCRIPTEN__
    processInput();
#else
    processInput(m_window.get());
#endif
}

bool Renderer::CloseWindow()
{
#ifdef __EMSCRIPTEN__
    return closeWindow;
#else
    return glfwWindowShouldClose(m_window.get());
#endif
}


void BoxRenderer::Draw(const Shader &s, const glm::vec3 &position,
    const glm::vec3 &size,const glm::vec3 color)
{
    s.Use();

    glm::mat4 model = glm::mat4(1.0f);

    // Translate
    model = glm::translate(model, position);

    // Rotate
#ifdef __EMSCRIPTEN__
    model = glm::rotate(model, glm::radians((float)(emscripten_get_now() * 0.001) * 15.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
#else
    model = glm::rotate(model, glm::radians((float)glfwGetTime() * 15.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
#endif

    // Scale
    model = glm::scale(model, size);

    s.SetMatrix4("model", model, true);

    glBindVertexArray(this->_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}

// Box Renderer specific functions:

BoxRenderer::BoxRenderer()
{
    glGenVertexArrays(1, &_VAO);
}

void BoxRenderer::initRenderer()
{
    unsigned int VBO;

    float vertices[] = {
        // Object Coord         // Tex Coord  // Normal Coord
        // Back face
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,   0.0f,  0.0f, -1.0f,// Bottom-left
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,   0.0f,  0.0f, -1.0f,// top-right
        0.5f, -0.5f, -0.5f,     1.0f, 0.0f,   0.0f,  0.0f, -1.0f,// bottom-right         
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,   0.0f,  0.0f, -1.0f,// top-right
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,   0.0f,  0.0f, -1.0f,// bottom-left
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,   0.0f,  0.0f, -1.0f,// top-left
        // Front face
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,   0.0f,  0.0f, 1.0f,// bottom-left
        0.5f, -0.5f,  0.5f,     1.0f, 0.0f,   0.0f,  0.0f, 1.0f,// bottom-right
        0.5f,  0.5f,  0.5f,     1.0f, 1.0f,   0.0f,  0.0f, 1.0f,// top-right
        0.5f,  0.5f,  0.5f,     1.0f, 1.0f,   0.0f,  0.0f, 1.0f,// top-right
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,   0.0f,  0.0f, 1.0f,// top-left
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,   0.0f,  0.0f, 1.0f,// bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,// top-right
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,   -1.0f,  0.0f,  0.0f,// top-left
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,// bottom-left
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,// bottom-left
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,// bottom-right
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,// top-right
        // Right face
        0.5f,  0.5f,  0.5f,     1.0f, 0.0f,   1.0f,  0.0f,  0.0f,// top-left
        0.5f, -0.5f, -0.5f,     0.0f, 1.0f,   1.0f,  0.0f,  0.0f,// bottom-right
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,   1.0f,  0.0f,  0.0f,// top-right         
        0.5f, -0.5f, -0.5f,     0.0f, 1.0f,   1.0f,  0.0f,  0.0f,// bottom-right
        0.5f,  0.5f,  0.5f,     1.0f, 0.0f,   1.0f,  0.0f,  0.0f,// top-left
        0.5f, -0.5f,  0.5f,     0.0f, 0.0f,   1.0f,  0.0f,  0.0f,// bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,   0.0f, -1.0f,  0.0f,// top-right
        0.5f, -0.5f, -0.5f,     1.0f, 1.0f,   0.0f, -1.0f,  0.0f,// top-left
        0.5f, -0.5f,  0.5f,     1.0f, 0.0f,   0.0f, -1.0f,  0.0f,// bottom-left
        0.5f, -0.5f,  0.5f,     1.0f, 0.0f,   0.0f, -1.0f,  0.0f,// bottom-left
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,   0.0f, -1.0f,  0.0f,// bottom-right
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,   0.0f, -1.0f,  0.0f,// top-right
        // Top face
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,   0.0f,  1.0f,  0.0f,// top-left
        0.5f,  0.5f,  0.5f,     1.0f, 0.0f,   0.0f,  1.0f,  0.0f,// bottom-right
        0.5f,  0.5f, -0.5f,     1.0f, 1.0f,   0.0f,  1.0f,  0.0f,// top-right     
        0.5f,  0.5f,  0.5f,     1.0f, 0.0f,   0.0f,  1.0f,  0.0f,//bottom-right
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,   0.0f,  1.0f,  0.0f,// top-left
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,   0.0f,  1.0f,  0.0f// bottom-left  
    };


    glGenBuffers(1, &VBO);

    glBindVertexArray(this->_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // Set the object coord
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    
    // Set the tex coord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // Set the normal coord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

    glBindVertexArray(0);
}