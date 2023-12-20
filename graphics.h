#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <stack>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "skyboxshader.h"
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

    // Skybox
    GLuint skyboxVAO;
    GLuint skyboxVBO;
    SkyboxShader* m_skyboxshader;
    GLint m_skyboxProjMatrix;
    GLint m_skyboxViewMatrix;
    GLint m_skyboxPositionAttrib;
    std::vector<GLfloat> skyboxVertices;

    float matAmbient_sun[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float matDiff_sun[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float matSpec_sun[4] = { 1.0f, 1.0f, 1.0f, 1.0f, };
    float matShininess_sun = 1000000000.0f;

    float matAmbient_comet[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float matDiff_comet[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float matSpec_comet[4] = { 1.0f, 1.0f, 1.0f, 1.0f, };
    float matShininess_comet = 10000.0f;

    float matAmbient_rock[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    float matDiff_rock[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float matSpec_rock[4] = { 0.5f, 0.5f, 0.5f, 1.0f, };
    float matShininess_rock = 15.0f;

    float matAmbient_gas[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
    float matDiff_gas[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    float matSpec_gas[4] = { 0.5f, 0.5f, 0.5f, 1.0f, };
    float matShininess_gas = 15.0f;

    float matAmbient_ship[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
    float matDiff_ship[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float matSpec_ship[4] = { 0.5f, 0.5f, 0.5f, 1.0f, };
    float matShininess_ship = 1.0f;

    void setMaterialSun();
    void setMaterialComet();
    void setMaterialRock();
    void setMaterialGas();
    void setMaterialShip();

    GLuint cubeMapTextureID;
    GLuint loadCubeMap();
    void setupSkybox();

    Sphere* m_sun;
    Sphere* m_comet;
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
    bool orbitIndex = -1;
    bool orbiting = false;
    glm::vec3 planetLoc[11];
};

#endif /* GRAPHICS_H */
