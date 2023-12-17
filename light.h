#ifndef LIGHT_H
#define LIGHT_H

#include "graphics_headers.h"

class Light {
public:
	GLfloat m_globalAmbient[4];
	GLfloat m_lightAmbient[4];
	GLfloat m_lightDiffuse[4];
	glm::vec3 m_lightPosition;
	float m_lightPositionViewSpace[3];
	GLfloat m_lightSpecular[4];

	void Update(glm::mat4 viewMatrix);
	Light(glm::mat4 viewMatrix, glm::vec3 lightPos, float lightAmb[4], float lightDiff[4], float lightSpec[4], float gAmb[4]);
};

#endif //LIGHT_H