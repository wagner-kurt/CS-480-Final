#include "camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

bool Camera::Initialize(int w, int h)
{
  //--Init the view and projection matrices
  //  if you will be having a moving camera the view matrix will need to more dynamic
  //  ...Like you should update it before you render more dynamic 
  //  for this project having them static will be fine
  //view = glm::lookAt( glm::vec3(x, y, z), //Eye Position
  //                    glm::vec3(0.0, 0.0, 0.0), //Focus point
  //                    glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
    focalPoint = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraPos = glm::vec3(0.0f, 10.0f, -16.0f);
    cameraFront = glm::normalize(-cameraPos);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    FoV = 40.f;
    width = w;
    height = h;

  //view = glm::lookAt(cameraPos, focalPoint, cameraUp);
  view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);

  projection = glm::perspective( glm::radians(FoV), //the FoV typically 90 degrees is good which is what this is set to
                                 float(width)/float(height), //Aspect Ratio, so Circles stay Circular
                                 0.01f, //Distance to the near plane, normally a small value like this
                                 100.0f); //Distance to the far plane, 
  return true;
}
/*
void Camera::Update(glm::vec3 pan) {
    //pan camera by changing the focal point
    focalPoint += pan;
    view = glm::lookAt(cameraPos, focalPoint, cameraUp);
}

void Camera::Update(double dX, double dY) {
    //rotate camera view angle, divide mouse position delta to reduce sensitivity
    view *= glm::rotate(glm::mat4(1.0f), (float)dX / 100.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    view *= glm::rotate(glm::mat4(1.0f), (float)dY / 100.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}
*/
void Camera::Move(int direction) {
    glm::vec3 moveVec = glm::vec3(0.0f, 0.0f, 0.0f);
    //move camera position and focal point L/R/forward/backward
    switch (direction) {
    case CAM_FORWARD:
        moveVec = cameraFront;
        break;
    case CAM_BACKWARD:
        moveVec = -cameraFront;
        break;
    case CAM_LEFT:
        moveVec = -glm::normalize(glm::cross(cameraFront, cameraUp));
        break;
    case CAM_RIGHT:
        moveVec = glm::normalize(glm::cross(cameraFront, cameraUp));
        break;
    }

    cameraPos += moveVec * 0.1f;
    focalPoint += moveVec * 0.1f;

    view = glm::lookAt(cameraPos, focalPoint, cameraUp);
}

void Camera::Rotate(double dX, double dY) {
    //cameraFront.x += (float)dX * 0.1f;
    //cameraFront.y += (float)dY * 0.1f;

    //view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);
}

void Camera::Zoom(double dY) {
    FoV = glm::clamp(FoV - (float)dY, 10.0f, 100.0f);

    projection = glm::perspective(glm::radians(FoV), float(width) / float(height), 0.01f, 100.0f);
}

glm::mat4 Camera::GetProjection()
{
  return projection;
}

glm::mat4 Camera::GetView()
{
  return view;
}
