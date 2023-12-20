#ifndef SKYBOXSHADER_H
#define SKYBOXSHADER_H

#include "shader.h"

class SkyboxShader : public Shader {
public:
    SkyboxShader();
    ~SkyboxShader();
    bool Initialize();
    void Enable();
    bool AddShader(GLenum ShaderType);
    bool Finalize();
    GLint GetUniformLocation(const char* pUniformName);
    GLint GetAttribLocation(const char* pAttribName);
    GLuint GetShaderProgram() { return m_shaderProg; }

private:
    GLuint m_shaderProg;
    std::vector<GLuint> m_shaderObjList;
};

#endif //SKYBOXSHADER_H