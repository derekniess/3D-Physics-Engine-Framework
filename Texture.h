#pragma once
#include <GL/glew.h>
#include <memory>
class Texture
{
private:
	int width;
	int height;
	GLuint TextureID;
	std::unique_ptr<unsigned char> PixelArray;

public:
	Texture(int aWidth, int aHeight, unsigned char * aPixel, int aID) : width(aWidth), height(aHeight), PixelArray(aPixel), TextureID(aID) {}
	~Texture() {};

	inline int GetWidth() { return width; }
	inline int GetHeight() { return height; }
	inline GLuint GetTextureID() { return TextureID; }
	inline unsigned char * GetPixels() { return PixelArray.get(); }
};