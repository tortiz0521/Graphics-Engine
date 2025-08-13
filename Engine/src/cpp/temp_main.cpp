#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
//#include "../headers/stb_image.h"

#include "../headers/resource_manager.h"
#include "../headers/renderer.h"
#include "../headers/camera.h"

// General settings
constexpr unsigned int WIDTH = 800;
constexpr unsigned int HEIGHT = 600;

// Settings for camera:
constexpr glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 10.0f);
constexpr glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
constexpr glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr float yaw = -90.0f, pitch = 0.0f;
constexpr float moveSpeed = 2.5f, mouseSense = 0.1f;
constexpr float fov = 45.0f;

// Camera object istelf.
Camera cam(camPos, camFront, camUp, moveSpeed, mouseSense, pitch, yaw, fov);

// Delta time.
float dt = 0.0f;
// Mouse setup:
bool firstMouse = true;
float lastX = (float)WIDTH / 2.0f, lastY = (float)HEIGHT / 2.0f;

bool setupWindow(const unsigned int w, const unsigned int h, GLFWwindow* &window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);

int main()
{
    GLFWwindow *window, *current;
    if(!setupWindow(WIDTH, HEIGHT, window)) {
        std::cout << "ERROR::WINDOW_FAILED_TO_LOAD\n";
        glfwTerminate();
        return -1;
    }

    current = glfwGetCurrentContext();
    if (current != window)
        std::cout << "HERE";

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity,
            GLsizei length, const GLchar* message, const void* userParam)
        {
            std::cerr << "OpenGL DEBUG: " << message << std::endl;
        }, nullptr);

    // Setup the viewport!
    glViewport(0, 0, WIDTH, HEIGHT);
    ResourceManager RM = ResourceManager();
    std::shared_ptr<Texture> tex = RM.LoadTexture("../../assets/textures/container.jpg", NONE);

    // Update the resource manager:
    Shader box = RM.LoadShader("../../assets/shaders/box.vs",
        "../../assets/shaders/box.fs", "box");
    box.ID();

    BoxRenderer BOX;
    BOX.initRenderer();

    // Set callbacks:
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Tell OpenGL to use the z-buffer for depth tests!
    glEnable(GL_DEPTH_TEST);

    // Tell the window to hide the cursor and 'capture' it to the screen.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    float last = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float current = glfwGetTime();
        dt = current - last;
        last = current;

        glfwPollEvents();

        glClearColor(0.3f, 0.7f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(cam.GetFOV()),
            (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        box.Use();
        box.SetMatrix4("projection", projection);
        box.SetMatrix4("view", cam.GetView());
        glActiveTexture(GL_TEXTURE0);
        tex.get()->Bind();
        box.SetInteger("tex", 0);
        BOX.Draw(box, glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(5.0f), glm::vec3(0.0f), window);

        glfwSwapBuffers(window);
    }

}

bool setupWindow(const unsigned int w, const unsigned int h, GLFWwindow* &window)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    window = glfwCreateWindow(800, 600, "Engine", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window); // Makes the context of our window the main context on the current thread!

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // Set up GLAD. Uses your OS specific OpenGL functions!
    {
        std::cout << "Failed to initialize GLAD!" << std::endl;
        return false;
    }

    return true;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    // When the user presses the escape key, set the WindowShouldClose function to true.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    std::cout << (action == GLFW_REPEAT ? "REPEAT" : "");
    
    // Get key inputs and determine if they are presses or releases.
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        cam.MoveCamera(FORWARD, dt);
    }
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        cam.MoveCamera(LEFT, dt);
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        cam.MoveCamera(BACKWARD, dt);
    }
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        cam.MoveCamera(RIGHT, dt);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    cam.ProcessMouse(xoffset, yoffset, true);
}
