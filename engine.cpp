

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

    // Check if need to toggle third person
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (!keyPressed)
        {
            if (thirdPerson)
            {
                thirdPerson = false;
                glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else
            {
                thirdPerson = true;
                glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                velocity = 0.f;
            }

            orbiting = false;
            m_graphics->ToggleOrbit(orbiting);
            m_graphics->ToggleView(thirdPerson);
            keyPressed = true;
        }
    }
    else if (keyPressed && glfwGetKey(m_window->getWindow(), GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
        keyPressed = false;

    if (thirdPerson)
    {
        // Third person mode inputs
        float movement = 0.00025f;
        double dX = 0.f;
        double dY = 0.f;
        double tilt = 0.f;

        // move camera inputs
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
            velocity += movement;
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
            velocity -= movement;
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
            dX += 1.f;
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
            dX -= 1.f;
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_E) == GLFW_PRESS)
            dY -= 1.f;
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_Q) == GLFW_PRESS)
            dY += 1.f;
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_C) == GLFW_PRESS)
            tilt += 1.f;
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_Z) == GLFW_PRESS)
            tilt -= 1.f;
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            velocity = 0.f;

        m_graphics->getCamera()->Move(CAM_FORWARD, velocity);

        // Keep mouse position stored so transition is smooth
        glfwGetCursorPos(m_window->getWindow(), &mouseX, &mouseY);

        // set sensitivity here
        float sensitivity = 0.5f;
        dX *= sensitivity;
        dY *= sensitivity;
        tilt *= sensitivity;

        // rotate camera inputs if key press detected
        if (dX != 0.0 || dY != 0.0 || tilt != 0.0) {
            m_graphics->getCamera()->Rotate(dX, dY, tilt);
        }
    }
    else
    {
        // first person mode inputs

        float movement = 0.025f;

        // move camera inputs
        if (!orbiting) {
            if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
                m_graphics->getCamera()->Move(CAM_FORWARD, movement);
            if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
                m_graphics->getCamera()->Move(CAM_BACKWARD, movement);
            if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
                m_graphics->getCamera()->Move(CAM_LEFT, movement);
            if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
                m_graphics->getCamera()->Move(CAM_RIGHT, movement);
        }

        // toggle planet panning mode
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            if (orbiting && !keyPressed) {
                m_graphics->getCamera()->Reset();
                orbiting = false;
                keyPressed = true;
                m_graphics->ToggleOrbit(orbiting);
            }
            else if (!keyPressed) {
                orbiting = true;
                keyPressed = true;
                m_graphics->ToggleOrbit(orbiting);
            }
        }
           
        // Update camera animation here.
        // obtain and store mouse position
        glfwGetCursorPos(m_window->getWindow(), &mouseX, &mouseY);
        double dX = -(mouseX - prevMouseX);
        double dY = mouseY - prevMouseY;

        //set sensitivity here
        float sensitivity = 0.1f;
        dX *= sensitivity;
        dY *= sensitivity;

        //rotate camera inputs if difference in mouse position detected
        if (dX != 0.0 || dY != 0.0) {
            m_graphics->getCamera()->Rotate(dX, dY, 0.f);
        }
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
