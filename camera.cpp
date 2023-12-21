#include "camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

bool Camera::Initialize(int w, int h)
{
    focalPoint = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraPos = glm::vec3(0.0f, 10.0f, -16.0f);
    cameraFront = glm::normalize(-cameraPos);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    FoV = 40.f;
    width = w;
    height = h;
    thirdPer = false;
    yaw = 90.f;
    pitch = -25.f;
    roll = 0.f;

    camDist = 2.5f;

  view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);

  projection = glm::perspective( glm::radians(FoV), //the FoV typically 90 degrees is good which is what this is set to
                                 float(width)/float(height), //Aspect Ratio, so Circles stay Circular
                                 0.01f, //Distance to the near plane, normally a small value like this
                                 100.0f); //Distance to the far plane, 
  return true;
}

void Camera::Move(int direction, float velocity) {
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

    cameraPos += moveVec * velocity;

    if (thirdPer)
        view = glm::lookAt(cameraPos - (cameraFront * camDist), cameraPos, cameraUp);
    else
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::Rotate(double dX, double dY, double tilt) {
    yaw -= dX;
    pitch -= dY;
    roll -= tilt;

    if(pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    // adjust front coords based on yaw and pitch
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    // adjust cameraUp based on roll
    glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(roll), cameraFront);
    cameraUp = glm::mat3(roll_mat) * glm::vec3(0.f, 1.f, 0.f);

    if (thirdPer)
        view = glm::lookAt(cameraPos - (cameraFront * camDist), cameraPos, cameraUp);
    else
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
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

glm::mat4 Camera::GetOrigView()
{
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::vec3 Camera::GetPosition()
{
    return cameraPos;
}
void Camera::SetPosition(glm::vec3 pos)
{
    cameraPos = pos;
    if (thirdPer)
        view = glm::lookAt(cameraPos - (cameraFront * camDist), cameraPos, cameraUp);
    else
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::ToggleView(bool thrPer)
{
    thirdPer = thrPer;

    // update view matrix
    if (thirdPer)
        view = glm::lookAt(cameraPos - (cameraFront * camDist), cameraPos, cameraUp);
    else
    {
        roll = 0.f;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void Camera::Reset()
{
    if (thirdPer)
        view = glm::lookAt(cameraPos - (cameraFront * camDist), cameraPos, cameraUp);
    else
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}