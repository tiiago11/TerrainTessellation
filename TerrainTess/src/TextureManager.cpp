//**********************************************
//Singleton Texture Manager class
//Written by Ben English
//benjamin.english@oit.edu
//
//For use with OpenGL and the FreeImage library
//**********************************************

#include "TextureManager.h"

#include <iostream>

using namespace std; 
std::map<int, TexInfo_t> TextureManager::m_TexMap;

TextureManager* TextureManager::m_inst(0);

TextureManager* TextureManager::Inst()
{
	if (!m_inst) {
		m_inst = new TextureManager();
	}

	return m_inst;
}

TextureManager::TextureManager()
{
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif
}

TextureManager::~TextureManager()
{
	// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif
	m_inst = 0;
}

GLuint TextureManager::LoadTexture(const char* filename, int id)
{
	// Determine the format of the image.
	// Note: The second paramter ('size') is currently unused, and we should use 0 for it.
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);

	// Image not found? Abort! Without this section we get a 0 by 0 image with 0 bits-per-pixel but we don't abort, which
	// you might find preferable to dumping the user back to the desktop.
	if (format == FIF_UNKNOWN)
	{
		cout << "Could not find image or format not recognized: " << filename << " - Aborting." << endl;
		return false;
	}

	// If we're here we have a known image format, so load the image into a bitap
	FIBITMAP* bitmap = FreeImage_Load(format, filename);

	// How many bits-per-pixel is the source image?
	int bitsPerPixel = FreeImage_GetBPP(bitmap);


	cout << "Color type: ";

	switch (FreeImage_GetColorType(bitmap))
	{
	case FIC_MINISWHITE:
		cout << "FIC_MINISWHITE";
		break;
	case FIC_MINISBLACK:
		cout << "FIC_MINISBLACK";
		break;
	case FIC_RGB:
		cout << "FIC_RGB";
		break;
	case FIC_PALETTE:
		cout << "FIC_PALETTE";
		break;
	case FIC_RGBALPHA:
		cout << "FIC_RGBALPHA";
		break;
	case FIC_CMYK:
		cout << "FIC_CMYK";
		break;
	default:
		break;
	}

	cout << endl;

	cout << "Image type: ";
	printImageType(FreeImage_GetImageType(bitmap));

	FIBITMAP* convertedbitmap = FreeImage_ConvertToStandardType(bitmap, true);

	FreeImage_Unload(bitmap);

	bitmap = convertedbitmap;

	cout << "Converted image type: ";
	printImageType(FreeImage_GetImageType(bitmap));

	// Convert our image up to 32 bits (8 bits per channel, Red/Green/Blue/Alpha) -
	// but only if the image is not already 32 bits (i.e. 8 bits per channel).
	// Note: ConvertTo32Bits returns a CLONE of the image data - so if we
	// allocate this back to itself without using our bitmap32 intermediate
	// we will LEAK the original bitmap data, and valgrind will show things like this:
	//
	// LEAK SUMMARY:
	//  definitely lost: 24 bytes in 2 blocks
	//  indirectly lost: 1,024,874 bytes in 14 blocks    <--- Ouch.
	//
	// Using our intermediate and cleaning up the initial bitmap data we get:
	//
	// LEAK SUMMARY:
	//  definitely lost: 16 bytes in 1 blocks
	//  indirectly lost: 176 bytes in 4 blocks
	//
	// All above leaks (192 bytes) are caused by XGetDefault (in /usr/lib/libX11.so.6.3.0) - we have no control over this.
	//
	FIBITMAP* bitmap32;
	if (bitsPerPixel == 32)
	{
		cout << "Source image has " << bitsPerPixel << " bits per pixel. Skipping conversion." << endl;
		bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
	}
	else
	{
		cout << "Source image has " << bitsPerPixel << " bits per pixel. Converting to 32-bit colour." << endl;
		bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
		if (bitmap32 == NULL)
		{
			cout << "Unable to convert image to 32bits." << endl;
			bitmap32 = FreeImage_ConvertTo24Bits(bitmap);
			if (bitmap32 == NULL)
			{
				cout << "Unable to convert image to 24bits." << endl;
				return false;
			}
		}
	}

	// Some basic image info - strip it out if you don't care
	int imageWidth = FreeImage_GetWidth(bitmap32);
	int imageHeight = FreeImage_GetHeight(bitmap32);
	cout << "Image: " << filename << " is size: " << imageWidth << "x" << imageHeight << "." << endl;

	// Get a pointer to the texture data as an array of unsigned bytes.
	// Note: At this point bitmap32 ALWAYS holds a 32-bit colour version of our image - so we get our data from that.
	// Also, we don't need to delete or delete[] this textureData because it's not on the heap (so attempting to do
	// so will cause a crash) - just let it go out of scope and the memory will be returned to the stack.
	GLubyte* textureData = FreeImage_GetBits(bitmap32);

	GLuint gl_texID;
	glGenTextures(1, &gl_texID);
	glBindTexture(GL_TEXTURE_2D, gl_texID);
	glTexImage2D(GL_TEXTURE_2D,    // Type of texture
		0,                // Mipmap level (0 being the top level i.e. full size)
		GL_RGBA,          // Internal format
		imageWidth,       // Width of the texture
		imageHeight,      // Height of the texture,
		0,                // Border in pixels
		GL_BGRA,		  // Data format
		GL_UNSIGNED_BYTE, // Type of texture data
		textureData);     // The image data to use for this texture

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//// add to the map
	TexInfo_t info;
	info.filename = filename;
	info.glId = gl_texID;
	info.size = glm::vec2(imageWidth, imageHeight);
	TextureManager::m_TexMap.emplace(std::pair<int, TexInfo_t>(id, info));
	////


	// Check for OpenGL texture creation errors
	GLenum glError = glGetError();
	if (glError)
	{
		cout << "There was an error loading the texture: " << filename << endl;

		switch (glError)
		{
		case GL_INVALID_ENUM:
			cout << "Invalid enum." << endl;
			break;

		case GL_INVALID_VALUE:
			cout << "Invalid value." << endl;
			break;

		case GL_INVALID_OPERATION:
			cout << "Invalid operation." << endl;

		default:
			cout << "Unrecognised GLenum." << endl;
			break;
		}

		cout << "See https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml for further details." << endl;
	}

	// Unload the 32-bit colour bitmap
	FreeImage_Unload(bitmap32);

	// If we had to do a conversion to 32-bit colour, then unload the original
	// non-32-bit-colour version of the image data too. Otherwise, bitmap32 and
	// bitmap point at the same data, and that data's already been free'd, so
	// don't attempt to free it again! (or we'll crash).
	if (bitsPerPixel != 32)
	{
		FreeImage_Unload(bitmap);
	}

	return gl_texID;
}




void TextureManager::printImageType(FREE_IMAGE_TYPE type) {
	switch (type)
	{
	case FIT_UNKNOWN:
		std::cout << "FIT_UNKNOWN";
		break;
	case FIT_BITMAP:
		std::cout << "FIT_BITMAP";
		break;
	case FIT_UINT16:
		std::cout << "FIT_UINT16";
		break;
	case FIT_INT16:
		std::cout << "FIT_INT16";
		break;
	case FIT_UINT32:
		std::cout << "FIT_UINT32";
		break;
	case FIT_INT32:
		std::cout << "FIT_INT32";
		break;
	case FIT_FLOAT:
		std::cout << "FIT_FLOAT";
		break;
	case FIT_DOUBLE:
		std::cout << "FIT_DOUBLE";
		break;
	case FIT_COMPLEX:
		std::cout << "FIT_COMPLEX";
		break;
	case FIT_RGB16:
		std::cout << "FIT_RGB16";
		break;
	case FIT_RGBA16:
		std::cout << "FIT_RGBA16";
		break;
	case FIT_RGBF:
		std::cout << "FIT_RGBF";
		break;
	case FIT_RGBAF:
		std::cout << "FIT_RGBAF";
		break;
	default:
		break;
	}
	std::cout << endl;
}


void TextureManager::bindTexture(int texID)
{
	TexInfo_t info = getTexInfo(texID);
	if(info.glId > 0)
		glBindTexture(GL_TEXTURE_2D, info.glId);
}

TexInfo_t TextureManager::getTexInfo(int texID) {
	TexInfo_t info;
	try
	{
		info = m_TexMap.at(texID);
	}
	catch (std::out_of_range& const e)
	{
		std::cerr << e.what() << std::endl;
		info.glId = 0;
	}

	return info;
}

