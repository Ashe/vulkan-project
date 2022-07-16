#include "App.h"

App::App(const char* title, unsigned int width, unsigned int height) {

  // Signal that application is ready
  printf("Initialising %s..\n", title);

  // Initialise GLFW and create window
  window = App::initialiseGLFW(this, title, width, height);
  if (!window) {
    return;
  }

  // Initialise Vulkan
  const bool vulkanInitResult = App::initialiseVulkanInstance(vulkanInstance, title);
  if (!vulkanInitResult) {
    return;
  }

  // Signal that application is ready
  printf("Initialisation successful.\n");
  isInitialised = true;
}

App::~App() {

  // Signal that the application is about to terminate
  printf("Terminating application..\n");

  // Destroy Vulkan instance
  vkDestroyInstance(vulkanInstance, nullptr);

  // Destroy window
  if (window != nullptr) {
    glfwDestroyWindow(window);
  }

  // Terminate glfw
  glfwTerminate();

  // Signal that we reached the end of termination
  printf("Terminating successful. Goodbye.\n");
  isInitialised = false;
}

void App::run() {

  // Only run application if is fully initialised
  if (!isInitialised) {
    return;
  }
  
  // Prepare to loop application
  double previous_time = glfwGetTime();

  // Keep the application running as long as there's a window
  while (!glfwWindowShouldClose(window))
  {
    // Calculate delta time
    const double current_time = glfwGetTime();
    const double delta_time = current_time - previous_time;
    previous_time = current_time;

    // Handle any events that occurred since last frame
    glfwPollEvents();

    // @TODO: Continue writing application loop

    // Render application
    glfwSwapBuffers(window);
  }
}

void App::key_callback(int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    printf("Key '%d' pressed!\n", key);
  }
}

//////////////////////////////
// Initialisation functions //
//////////////////////////////

GLFWwindow* App::initialiseGLFW(App* app, const char* title, unsigned int width, unsigned int height) {

  // Initialise GLFW
  const int initResult = glfwInit();
  if (initResult != GLFW_TRUE) {
    printf("Error: Failed to initialise GLFW.\n");
    return nullptr;
  }

  // Create a window with GLFW
  GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window) {
    printf("Error: Failed to create window.\n");
    return nullptr;
  }

  // Give GLFW a reference to this application
  glfwSetWindowUserPointer(window, app);

  // Setup callbacks
  glfwSetErrorCallback(&App::handle_error_callback);
  glfwSetKeyCallback(window, &App::handle_key_callback);

  // Return the newly created window, if any
  return window;
}

bool App::initialiseVulkanInstance(VkInstance& vulkanInstance, const char* title) {

  // Define application information
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = title;
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  // Specify instance creation information
  uint32_t glfwExtensionCount;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;
  createInfo.enabledLayerCount = 0;

  // Create the Vulkan instance
  const VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanInstance);
  if (result != VK_SUCCESS) {
    printf("Error: Failed to create VkInstance.\n");
    return false;
  }

  // Get available extensions
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

  // Determine if there are any missing extensions
  std::vector<VkExtensionProperties> missingExtensions;
  std::vector<VkExtensionProperties> requiredExtensions(glfwExtensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &glfwExtensionCount, requiredExtensions.data());
  for (const VkExtensionProperties& requiredExtension : requiredExtensions) {
    bool found = false;
    for (int i = 0; i < availableExtensions.size(); ++i) {
      const int strCmp = strcmp(requiredExtension.extensionName, availableExtensions[i].extensionName);
      const bool versionCmp = requiredExtension.specVersion <= availableExtensions[i].specVersion;
      if (strCmp == 0 && versionCmp) {
        found = true;
      }
    }
    if (!found) {
      missingExtensions.push_back(requiredExtension);
    }
  }

  // If any extensions are missing
  if (!missingExtensions.empty()) {

    // List missing extensions
    printf("Error: Missing vulkan extensions:\n");
    for (const VkExtensionProperties& missingExtension : missingExtensions) {
      printf("\t%s (v. %u)\n", missingExtension.extensionName, missingExtension.specVersion);
    }

    // List available extensions
    printf("Available extensions:\n");
    for (const VkExtensionProperties& availableExtension : availableExtensions) {
      printf("\t%s (v. %u)\n", availableExtension.extensionName, availableExtension.specVersion);
    }

    // Fail initialisation
    return false;
  }

  // Initialisation successful
  return true;
}

/////////////////////////////
// Static helper functions //
/////////////////////////////

void App::handle_error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

void App::handle_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
  if (app != nullptr) {
    app->key_callback(key, scancode, action, mods);
  }
}
