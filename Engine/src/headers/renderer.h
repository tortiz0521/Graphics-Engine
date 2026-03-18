#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include <memory>
#include <glad/glad.h>
#include <typeinfo>
#include <functional>

#include "resource_manager.h"
#include "_ubo.h"
#include "../ECS/systems/camera_system.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


struct GLFWwindowDeleter {
    void operator()(GLFWwindow* window)
    {
        if (window) {
            glfwDestroyWindow(window);
        }
    }
};


namespace {
    static bool anon_firstMouse = true;
    static float anon_lastX = 0.0f;
    static float anon_lastY = 0.0f;

    static float dt = 0.0f, last = 0.0f;

    bool setupWindow(const unsigned int w, const unsigned int h, std::unique_ptr<GLFWwindow, GLFWwindowDeleter>& win)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        win = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(
            glfwCreateWindow(800, 600, "Engine", NULL, NULL)
        );
        if (win.get() == NULL)
        {
            std::cout << "Failed to create GLFW window!" << std::endl;
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(win.get()); // Makes the context of our window the main context on the current thread!

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // Set up GLAD. Uses your OS specific OpenGL functions!
        {
            std::cout << "Failed to initialize GLAD!" << std::endl;
            return false;
        }

        return true;
    };

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        // When the user presses the escape key, set the WindowShouldClose function to true.
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    };

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    };

    void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        if (anon_firstMouse)
        {
            anon_lastX = xpos;
            anon_lastY = ypos;
            anon_firstMouse = false;
        }

        float xoffset = xpos - anon_lastX;
        float yoffset = anon_lastY - ypos; // reversed since y-coordinates go from bottom to top

        anon_lastX = xpos;
        anon_lastY = ypos;

        CameraSystem::ProcessMouse(xoffset, yoffset, true);
    };

    void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            CameraSystem::MoveCamera(FORWARD, dt);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            CameraSystem::MoveCamera(LEFT, dt);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            CameraSystem::MoveCamera(BACKWARD, dt);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            CameraSystem::MoveCamera(RIGHT, dt);
    }
};


/*
THE PROBLEM WITH THIS RENDERER:
    While not necessarily a problem, there are definitely some interesting decisions I made...
    I think that there were some interesting ideas at play (the UBOs, the non-persistent orphaning),
    but I probably should have setup up my VBO idea a bit differently.

    My reason for an orphaned VBO: Instanced Arrays
    They are a really nice way to work with instancing, and it makes my batching algorithm a bit easier.
    I can run through a single array of memory, checking to make sure that we have all of our data 
    needed to instance. We change this a few times every frame, but maybe I should just have multiple VBOs?
    Some thoughts for the future, maybe...
*/


class Renderer
{
public:
    Renderer() = default;

    virtual void Draw(const LoadedModel &m, const Shader &s, const glm::vec3 &position,
        const glm::vec3 &size, const glm::vec3 color);

    void DrawInstanced(std::shared_ptr<LoadedModel> model, std::shared_ptr<Shader> shader, const std::vector<glm::mat4>& data);
    void InitRenderer(unsigned int WIDTH, unsigned int HEIGHT); 
    void InitRenderer(unsigned int WIDTH, unsigned int HEIGHT,
        void (*key_callback)(GLFWwindow*, int, int, int, int),
        void (*framebuffer_size_callback)(GLFWwindow*, int, int),
        void (*mouse_callback)(GLFWwindow*, int, int));


    void UpdateLightUBO(const LightVariant& light, size_t index);
    void UpdateCamUBO(const glm::mat4& camView);
    void UpdateCamProj(const glm::mat4& proj);
    void AddUBOStruct(const LightVariant& light);
    void ClearUBOs(); // Called on scene change!

    void SwapBuffers();
    void ProcessInput();
    unsigned int GetDynamicVBO();

private:
    std::unique_ptr<GLFWwindow, GLFWwindowDeleter> m_window{};
    unsigned int m_DynamicVBO{};

    // UBOs
    std::unique_ptr<UniformBuffer<DirectionLight>> m_dirUBO{};
    std::unique_ptr<UniformBuffer<SpotLight>> m_spotUBO{};
    std::unique_ptr<UniformBuffer<PointLight>> m_pointUBO{};
    std::unique_ptr<UniformBuffer<CameraData>> m_camUBO{};

    void UpdateVBO(const std::vector<glm::mat4>& data);
    bool CreateContext(unsigned int WIDTH, unsigned int HEIGHT);
};

class BoxRenderer : Renderer
{
public:
    BoxRenderer();
    void Draw(const Shader &s, const glm::vec3 &position,
        const glm::vec3 &size,const glm::vec3 color, GLFWwindow* curr);

    void initRenderer();
        
private:
    unsigned int _VAO{};
};

#endif // RENDERER_H