#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <algorithm>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <vector>

// Variables determined by Vulkan setup
struct VulkanSettings {
  VkInstance instance = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;
};

// Main class for application loop
class App {

public:

  // Constructors
  App(const char* title, unsigned int width, unsigned int height);
  ~App();

  // Run the application
  void run();

private:

  // Initialisation variables
  bool isInitialised = false;
  GLFWwindow* window = nullptr;
  VulkanSettings vulkan;

  // Callbacks
  void key_callback(int key, int scancode, int action, int mods);

  // Initialisation functions
  static GLFWwindow* initialiseGLFW(App* app, const char* title, unsigned int width, unsigned int height);
  static bool initialiseVulkanInstance(VulkanSettings& vulkan, const char* title);

  // Static helper functions
  static void handle_error_callback(int error, const char* description);
  static void handle_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
