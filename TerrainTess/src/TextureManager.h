//**********************************************
//Singleton Texture Manager class
//Written by Ben English
//benjamin.english@oit.edu
//
//For use with OpenGL and the FreeImage library
//**********************************************

#ifndef TextureManager_H
#define TextureManager_H

#include <windows.h>
#include "GL/glew.h"
#include "Freeimage/FreeImage.h"
#include <map>
#include <glm\glm.hpp>

typedef struct {
	const char* filename;
	GLuint glId;
	glm::vec2 size;
}TexInfo_t;

class TextureManager
{
public:
	static TextureManager* Inst();
	virtual ~TextureManager();

	GLuint LoadTexture(const char* filename, int id);
	static std::map<int, TexInfo_t> TextureManager::m_TexMap;
	void printImageType(FREE_IMAGE_TYPE type);
	void bindTexture(int texID);

private:
	TexInfo_t getTexInfo(int texID);

protected:
	TextureManager();
	static TextureManager* m_inst;
};

#endif
