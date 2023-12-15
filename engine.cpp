

#include "engine.h"
#include "glm/ext.hpp"

double Engine::scrollOffset = 0.0;

Engine::Engine(const char* name, int width, int height)
{
  m_WINDOW_NAME = name;
  m_WINDOW_WIDTH = width;
  m_WINDOW_HEIGHT = height;
}


Engine::~Engine()
{
  delete m_window;
  delete m_graphics;
  m_window = NULL;
  m_graphics = NULL;
}

bool Engine::Initialize()
{
  // Start a window
  m_window = new Window(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT);
  if(!m_window->Initialize())
  {
    printf("The window failed to initialize.\n");
    return false;
  }
  glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  // Start the graphics
  m_graphics = new Graphics();
  if(!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT))
  {
    printf("The graphics failed to initialize.\n");
    return false;
  }

  glfwSetCursorPosCallback(m_window->getWindow(), cursorPositionCallBack);
  glfwSetScrollCallback(m_window->getWindow(), scroll_callback);

  // No errors
  return true;
}

void Engine::Run()
{
  m_running = true;

  while (!glfwWindowShouldClose(m_window->getWindow()))
  {
      ProcessInput();
      Display(m_window->getWindow(), glfwGetTime());
      glfwPollEvents();
  }
  m_running = false;

}

void Engine::ProcessInput()
{
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window->getWindow(), true);


    // Update camera animation here.
    //obtain and store mouse position
    glfwGetCursorPos(m_window->getWindow(), &mouseX, &mouseY);
    double dX = -(mouseX - prevMouseX);
    double dY = mouseY - prevMouseY;
    
    //move camera inputs
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        m_graphics->getCamera()->Move(CAM_FORWARD);
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        m_graphics->getCamera()->Move(CAM_BACKWARD);
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        m_graphics->getCamera()->Move(CAM_LEFT);
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        m_graphics->getCamera()->Move(CAM_RIGHT);

    //set sensitivity here
    float sensitivity = 0.1f;
    dX *= sensitivity;
    dY *= sensitivity;

    //rotate camera inputs if difference in mouse position detected
    if (dX != 0.0 || dY != 0.0) {
        m_graphics->getCamera()->Rotate(dX, dY);
    }

    //change camera FoV from scroll wheel
    if (scrollOffset != 0.0) {
        m_graphics->getCamera()->Zoom(scrollOffset);
    }

    //update prev mouse pos for next frame
    prevMouseX = mouseX;
    prevMouseY = mouseY;

    scrollOffset = 0.0;
}

void Engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    scrollOffset = yoffset;
}

void Engine::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
 
    glfwGetCursorPos(window, &xpos, &ypos);
    std::cout << "Position: (" << xpos << ":" << ypos << ")";
}

unsigned int Engine::getDT()
{
  //long long TimeNowMillis = GetCurrentTimeMillis();
  //assert(TimeNowMillis >= m_currentTimeMillis);
  //unsigned int DeltaTimeMillis = (unsigned int)(TimeNowMillis - m_currentTimeMillis);
  //m_currentTimeMillis = TimeNowMillis;
  //return DeltaTimeMillis;
    return glfwGetTime();
}

long long Engine::GetCurrentTimeMillis()
{
  //timeval t;
  //gettimeofday(&t, NULL);
  //long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
  //return ret;
    return (long long) glfwGetTime();
}

void Engine::Display(GLFWwindow* window, double time) {

    m_graphics->Render();
    m_window->Swap();
    m_graphics->HierarchicalUpdate2(time);
}

static void cursorPositionCallBack(GLFWwindow* window, double xpos, double ypos) {
    //cout << xpos << " " << ypos << endl;
}
