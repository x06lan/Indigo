#include "window.hpp"

#include "log.hpp"
#include "exception.hpp"

/**
 * Error handling callback function for GLFW
 *
 * This will be called when there is an error for GLFW
 */
void GLFWErrorCallback(int, const char *errorMessage) {
    LOG_ERROR("{}", errorMessage);
}

/**
 * The GLFW API for scrolling only supports callbacks
 *
 * The callback function can't be a non-static method so this pointer `hack` is
 * necessary
 */
void GLFWScrollCallback(GLFWwindow *window, double offsetX, double offsetY) {
    static_cast<Window *>(glfwGetWindowUserPointer(window))->m_ScrollOffset = {
        offsetX, offsetY};
}

Window::Window(int width, int height, const char *title)
    : m_Width(width), m_Height(height), m_ScrollOffset(0) {
    LOG_TRACE("Creating Window");

    if (glfwInit() != GLFW_TRUE)
        throw std::runtime_error("Failed to Initialize GLFW");

    glfwWindowHint(GLFW_SAMPLES, 4); // uses 4x MSAA
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // force disables tiling on
                                                // tiling WMs such as i3 or sway

    m_Window = glfwCreateWindow(m_Width, m_Height, title, NULL, NULL);

    if (!m_Window)
        throw std::runtime_error("Failed to Create Window");

    // Binds the `this` pointer to the `m_Window` pointer so it can be
    // referenced in the callback function
    glfwSetWindowUserPointer(m_Window, this);

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1); // enable v-sync

    glfwSetInputMode(m_Window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwSetErrorCallback(GLFWErrorCallback);
    glfwSetScrollCallback(m_Window, GLFWScrollCallback);
}

Window::~Window() {
    LOG_TRACE("Deleting Window");
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Window::UpdateDimensions() {
    glfwGetWindowSize(m_Window, &m_Width, &m_Height);
}

void Window::UpdateCursorPosition() {
    glfwGetCursorPos(m_Window, &m_XPos, &m_YPos);
}

bool Window::GetKey(int key) const {
    return glfwGetKey(m_Window, key) == GLFW_PRESS;
}

bool Window::GetMouseButton(int button) const {
    return glfwGetMouseButton(m_Window, button);
}

glm::vec2 Window::GetCursorPos() const {
    double xPos, yPos;

    glfwGetCursorPos(m_Window, &xPos, &yPos);

    return glm::vec2{xPos, yPos};
}

glm::vec2 Window::GetCursorDelta() const {
    double xPos, yPos;

    glfwGetCursorPos(m_Window, &xPos, &yPos);

    return glm::vec2(xPos - m_XPos, yPos - m_YPos);
}

bool Window::ShouldClose() const {
    return GetKey(GLFW_KEY_ESCAPE) || GetKey(GLFW_KEY_Q) ||
           glfwWindowShouldClose(m_Window) == GLFW_TRUE;
}

void Window::PollEvents() {
    m_ScrollOffset = {0, 0};
    glfwPollEvents();
}
