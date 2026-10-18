#pragma once

#include <Windows.h>
struct GLFWwindow;

class Window {
public:
  Window(int width, int height, const char *title);
  ~Window();
  bool shouldClose();
  HWND getWin32Window();
  void loop();
  void terminate();
  bool isKeyDown(int key);

private:
  GLFWwindow *handler;
};
