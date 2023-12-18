#ifndef CAMERA_H
#define CAMERA_H

#include "graphics_headers.h"

#define CAM_FORWARD 1
#define CAM_BACKWARD 2
#define CAM_LEFT 3
#define CAM_RIGHT 4

class Camera
{
  public:
    Camera();
    ~Camera();
    bool Initialize(int w, int h);
    //void Update(glm::vec3 pan);
    //void Update(double dX, double dY);
    void Move(int direction, float velocity);
    void Rotate(double dX, double dY, double roll);
    void Zoom(double dY);
    glm::mat4 GetProjection();
    glm::mat4 GetView();
    glm::vec3 GetPosition();
    glm::vec3 GetFront();
    glm::mat4 GetOrigView();
    void ToggleView(bool thrPer);
    void Reset();
  
  private:
    int width, height;
    float FoV;
    glm::vec3 focalPoint;
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::mat4 projection;
    glm::mat4 view;
    bool thirdPer;
    float pitch;
    float yaw;
    float roll;
};

#endif /* CAMERA_H */
