#pragma once
#include "graphics_headers.h"
#include <SOIL2/SOIL2.h>

class Texture
{

public:
	Texture();
	Texture(const char* texFileName);
	Texture(const char* texFileName, const char* normFileName);
	bool loadTexture(const char* texFile);
	bool loadNormal(const char* normFile);
	GLuint getTextureID() { return m_TextureID; }
	GLuint getNormalID() { return m_NormalID; }

private:
	GLuint m_TextureID;
	GLuint m_NormalID;

	bool initializeTexture();

};

