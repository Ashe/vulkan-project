#include "App.h"

App::App(const char* title, int width, int height) :
    initialised(glfwInit()), 
    window(initialised ? 
        glfwCreateWindow(width, height, title, nullptr, nullptr) :
        nullptr) {

  // Stop constructing if glfw failed to initialise
  if (!initialised) {
    printf("Failed to initialise %s.\n", title);
    return;
  }

  // Signal that application is ready
  printf("Initialising %s..\n", title);

  // Give GLFW a reference to this application
  glfwSetWindowUserPointer(window, this);

  // Setup callbacks
  glfwSetErrorCallback(&App::handle_error_callback);
  glfwSetKeyCallback(window, &App::handle_key_callback);

  // Get extensions
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  printf("%d extensions supported.\n");

  // Signal that application is ready
  printf("%s initialised successfully.\n", title);
}

App::~App() {

  // Cleanup window
  if (window != nullptr) {
    glfwDestroyWindow(window);
  }

  // Only terminate glfw if it was initialised
  if (initialised) {
    glfwTerminate();
  }
}

void App::run() {

  // Only run when initialised
  if (!initialised) {
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
  printf("Key pressed!\n");
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
