#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <stack>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"
#include "sphere.h"
#include "mesh.h"
#include "light.h"

#define numVBOs 2;
#define numIBs 2;


class Graphics
{
  public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height);
    void HierarchicalUpdate2(double dt);
    void Render();

    Camera* getCamera() { return m_camera; }
    void ToggleView(bool thrPer);
    void ToggleOrbit(bool orb);

  private:
    std::string ErrorString(GLenum error);

    bool collectShPrLocs();
    void ComputeTransforms (double dt, std::vector<float> speed, std::vector<float> dist,
        std::vector<float> rotSpeed, glm::vec3 rotVector, std::vector<float> scale, 
        glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat);

    stack<glm::mat4> modelStack;

    Camera *m_camera;
    Shader *m_shader;

    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;
    GLint m_normalMatrix;
    GLint m_positionAttrib;
    GLint m_normalAttrib;
    GLint m_tcAttrib;
    GLint m_hasTexture;
    GLint hasN;
    GLint m_globalAmbLoc;
    GLint m_lightALoc;
    GLint m_lightDLoc;
    GLint m_lightSLoc;
    GLint m_lightPosLoc;
    GLint m_mAmbLoc;
    GLint m_mDiffLoc;
    GLint m_mSpecLoc;
    GLint m_mShineLoc;

    float matAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
    float matDiff[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float matSpec[4] = { 1.0f, 1.0f, 1.0f, 1.0f, };
    float matShininess = 20.0f;

    Sphere* m_sun;
    Sphere* m_mercury;
    Sphere* m_venus;
    Sphere* m_earth;
    Sphere* m_moon;
    Sphere* m_mars;
    Sphere* m_ceres;
    Sphere* m_jupiter;
    Sphere* m_saturn;
    Sphere* m_uranus;
    Sphere* m_neptune;

    Mesh* m_mesh;

    Light* m_light;

    bool thirdPer = false;
    int orbitIndex = -1;
    bool orbiting = false;
    glm::vec3 planetLoc[11];
};

#endif /* GRAPHICS_H */
