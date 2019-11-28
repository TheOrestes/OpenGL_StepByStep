
#include <iostream>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "TextureManager.h"
#include "UIManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
TextureManager::TextureManager()
{
	
}

//////////////////////////////////////////////////////////////////////////////////////////
TextureManager::~TextureManager()
{
	
}

//////////////////////////////////////////////////////////////////////////////////////////
GLint TextureManager::Load2DTextureFromFile(const std::string& path, const std::string& dir)
{
	//Generate texture ID and load texture data 
	std::string filename;
	filename = dir + '/' + path;

	void* data;
	int width, height, bpp;

	GLuint textureID;
	glGenTextures(1, &textureID);

	std::string msg;

	if (stbi_is_hdr(filename.c_str()))
	{
		stbi_set_flip_vertically_on_load(1);

		data = stbi_loadf(filename.c_str(), &width, &height, &bpp, 0);

		if (data != nullptr)
		{
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, (void*)data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);

			// Parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);

			msg = path + " loading successful...!";
			UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_DEBUG, "Texture", msg);
		}
		else
		{
			msg = path + " loading failed...!";
			UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_ERROR, "Texture", msg);
			textureID = -1;
		}
	}
	else
	{
		stbi_set_flip_vertically_on_load(1);
		data = stbi_load(filename.c_str(), &width, &height, &bpp, 0);

		if (data != nullptr)
		{
			// Assign texture to ID
			glBindTexture(GL_TEXTURE_2D, textureID);
			if (bpp > 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)data);

			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);

			// Parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D, 0);

			msg = path + " loading successful...!";
			UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_DEBUG, "Texture", msg);
		}
		else
		{
			msg = path + " loading failed...!";
			UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_ERROR, "Texture", msg);
			return -1;
		}
	}

	

	return textureID;
}

//////////////////////////////////////////////////////////////////////////////////////////
GLint TextureManager::LoadCubemapFromFile(const std::string& dir)
{
	// we work on the assumption that folder contains specifically named textures
	// forming a cubemap!
	// Do some book keeping first by storing textures in the folder into a vector
	std::vector<std::string> vecCubemapTextures;
	vecCubemapTextures.push_back(dir + '/' + "posx.jpg");
	vecCubemapTextures.push_back(dir + '/' + "negx.jpg");
	vecCubemapTextures.push_back(dir + '/' + "posy.jpg");
	vecCubemapTextures.push_back(dir + '/' + "negy.jpg");
	vecCubemapTextures.push_back(dir + '/' + "posz.jpg");
	vecCubemapTextures.push_back(dir + '/' + "negz.jpg");

	GLuint cubemapID;
	void* data;
	int width, height, bpp;

	stbi_set_flip_vertically_on_load(0);
	
	glGenTextures(1, &cubemapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

		for (int i = 0 ; i<vecCubemapTextures.size() ; i++)
		{
			data = stbi_load(vecCubemapTextures[i].c_str(), &width, &height, &bpp, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)data);
			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return cubemapID;
}




