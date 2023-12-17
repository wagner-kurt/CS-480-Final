#include "Texture.h"

Texture::Texture(const char* texFileName, const char* normFileName) {

	loadTexture(texFileName);
	loadNormal(normFileName);
	initializeTexture();
}

Texture::Texture(const char* texFileName) {
	loadTexture(texFileName);
	m_NormalID = 0;
	initializeTexture();
}

Texture::Texture() {
	m_TextureID = 0;
	m_NormalID = 0;
	printf("No Texture Data Provided.");
}

bool Texture::loadTexture(const char* texFile) {
	m_TextureID = SOIL_load_OGL_texture(texFile, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (!m_TextureID) {
		printf("Failed: Could not open texture file: %s\n", texFile);
		return false;
	}
	return true;
}

bool Texture::loadNormal(const char* normFile) {
	m_NormalID = SOIL_load_OGL_texture(normFile, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (!m_NormalID) {
		printf("Failed: Could not open normal file: %s\n", normFile);
		return false;
	}
	return true;
}

bool Texture::initializeTexture() {

	
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);
	
	return true;
}



