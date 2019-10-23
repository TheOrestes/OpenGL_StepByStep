
#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include <string>

enum TextureType
{
	TEXTURE_ALBEDO,
	TEXTURE_SPECULAR,
	TEXTURE_NORMAL,
	TEXTURE_HEIGHT,
	TEXTURE_OCCLUSION,
	TEXTURE_EMISSION,
	TEXTURE_MAX_TYPE
};

struct TextureProperty
{
	TextureProperty()
	{
		id = -1;
		name.clear();
		path.clear();
		hasTexture = false;
	}

	GLuint		id;
	std::string name;
	std::string path;
	bool		hasTexture;
	TextureType type;

	inline void setPath(const std::string& _path) { path = _path; };
	inline void setName(const std::string& _name) { name = _name; };
	inline void setID(const int _id) { id = _id; };
	inline void setType(const TextureType _type) { type = _type; }
	inline void SetHasTexture(bool _flag) { hasTexture = _flag;  }

	std::string getPath() { return path; };
	std::string getName() { return name; };
	GLuint		getID() { return id; };
	TextureType getType() { return type; }
	bool		getHasTexture() { return hasTexture;  }
};

struct Material
{
	// default constructor
	Material()
	{
		m_colAlbedo = glm::vec4(0.8,0.8,0.8,1.0);
		m_colRoughness = glm::vec4(glm::vec3(0.1), 1);
	}

	// Textures
	TextureProperty m_pTexAlbedo;
	TextureProperty m_pTexSpecular;
	TextureProperty m_pTexNormal;
	TextureProperty m_pTexHeight;
	TextureProperty m_pTexOcclusion;
	TextureProperty m_pTexEmission;

	// Material properties
	glm::vec4	m_colAlbedo;
	glm::vec4	m_colRoughness;
};

