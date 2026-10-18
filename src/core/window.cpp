#include "src/core/window.hpp"
#include "GLFW/glfw3.h"
#include <stdexcept>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

Window::Window(int width, int height, const char *title) {
  if (!glfwInit()) {
    throw std::runtime_error("failed to init glfw\n");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  this->handler = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (handler == nullptr) {
    throw std::runtime_error("failed to create glfw window\n");
  }
}

bool Window::shouldClose() { return glfwWindowShouldClose(handler); }

void Window::terminate() { glfwSetWindowShouldClose(handler, true); }

bool Window::isKeyDown(int key) {
  if (glfwGetKey(handler, key) == GLFW_PRESS) {
    return true;
  } else {
    return false;
  }
}

void Window::loop() { glfwPollEvents(); }

Window::~Window() {
  glfwDestroyWindow(handler);
  glfwTerminate();
}

HWND Window::getWin32Window() { return glfwGetWin32Window(handler); }
