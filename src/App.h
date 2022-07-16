#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "stdio.h"

// Main class for application loop
class App {

public:

  // Constructors
  App(const char* title, int width, int height);
  ~App();

  // Run the application
  void run();

private:

  // Initialisation variables
  const bool initialised = false;
  GLFWwindow* const window = nullptr;

  // Callbacks
  void key_callback(int key, int scancode, int action, int mods);

  // Static helper functions
  static void handle_error_callback(int error, const char* description);
  static void handle_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
