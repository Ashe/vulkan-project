#include "App.h"

namespace {
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
}

App::App(const char* title, unsigned int width, unsigned int height) {

  // Signal that application is ready
  printf("Initialising %s..\n", title);

  // Initialise GLFW and create window
  window = App::initialiseGLFW(this, title, width, height);
  if (!window) {
    return;
  }

  // Initialise Vulkan
  const bool vulkanInitResult = App::initialiseVulkanInstance(vulkan, title);
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

  // Destroy vulkan logical device
  if (vulkan.device) {
    vkDestroyDevice(vulkan.device, nullptr);
  }

  // Destroy Vulkan instance
  if (vulkan.instance) {
    vkDestroyInstance(vulkan.instance, nullptr);
  }

  // Destroy window
  if (window) {
    glfwDestroyWindow(window);
  }

  // Terminate glfw
  glfwTerminate();

  // Signal that we reached the end of termination
  printf("Termination successful. Goodbye.\n");
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

bool App::initialiseVulkanInstance(VulkanSettings& vulkan, const char* title) {

  // If we want to enable validation layers
  if (enableValidationLayers) {
    uint32_t layerCount;

    // Get available layers
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // Determine if any layers are missing
    std::vector<const char*> missingLayers;
    for (const char* requiredLayer : validationLayers) {

      bool layerFound = false;
      for (const VkLayerProperties& layer : availableLayers) {
        const int strCmp = strcmp(requiredLayer, layer.layerName);
        if (strCmp == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        missingLayers.push_back(requiredLayer);
      }
    }

    // If any layers are missing
    if (!missingLayers.empty())
    {
      printf("Error: Missing Vulkan validation layers:\n");
      for (const char* layer : missingLayers) {
        printf("\t%s\n", layer);
      }

      printf("Available validation layers:\n");
      for (const VkLayerProperties& layer : availableLayers) {
        printf("\t%s (v. %u)\n", layer.layerName, layer.specVersion);
      }
      return false;
    }
  }

  // Get extensions required for GLFW
  uint32_t glfwExtensionCount;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

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
    for (const VkExtensionProperties& extension : availableExtensions) {
      const int strCmp = strcmp(requiredExtension.extensionName, extension.extensionName);
      const bool versionCmp = requiredExtension.specVersion <= extension.specVersion;
      if (strCmp == 0 && versionCmp) {
        found = true;
        break;
      }
    }
    if (!found) {
      missingExtensions.push_back(requiredExtension);
    }
  }

  // If any extensions are missing
  if (!missingExtensions.empty()) {

    // List missing extensions
    printf("Error: Missing Vulkan extensions:\n");
    for (const VkExtensionProperties& extension : missingExtensions) {
      printf("\t%s (v. %u)\n", extension.extensionName, extension.specVersion);
    }

    // List available extensions
    printf("Available extensions:\n");
    for (const VkExtensionProperties& extension : availableExtensions) {
      printf("\t%s (v. %u)\n", extension.extensionName, extension.specVersion);
    }

    // Fail initialisation
    return false;
  }

  // Define application information
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = title;
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  // Specify instance creation information
  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;
  createInfo.enabledLayerCount = 0;

  // If validation layers are enabled, use them
  if (enableValidationLayers)
  {
    const uint32_t layerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.enabledLayerCount = layerCount;
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }

  // Create the Vulkan instance
  const VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkan.instance);
  if (result != VK_SUCCESS) {
    printf("Error: Failed to create VkInstance.\n");
    return false;
  }

  // Check how many physical devices are available
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vulkan.instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    printf("Error: No physical devices found.\n");
    return false;
  }

  // Determine most suitable device
  vulkan.physicalDevice = VK_NULL_HANDLE;
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(vulkan.instance, &deviceCount, devices.data());
  uint suitableDeviceRating = 0;
  for (const VkPhysicalDevice& device : devices) {

    // Prepare to rate device
    uint rating = 0;

    // Get features and properties of device
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      rating += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    rating += deviceProperties.limits.maxImageDimension2D;

    // If this device is better than the current device
    if (rating > suitableDeviceRating) {
      vulkan.physicalDevice = device;
      suitableDeviceRating = rating;
    }
  }

  // Ensure we found a suitable device
  if (vulkan.physicalDevice == VK_NULL_HANDLE) {
    printf("Error: Failed to find suitable physical device.\n");
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
