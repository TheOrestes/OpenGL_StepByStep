
#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include <string>

enum TextureType
{
	TEXTURE_ALBEDO,
	TEXTURE_MASK,
	TEXTURE_EMISSION,
	TEXTURE_NORMAL,
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
		m_colEmission = glm::vec4(0.0f);
		m_fRoughness = 0.5f;
		m_fMetallic = 0.5f;
		m_fOcclusion = 1.0f;
		m_fHeight = 0.0f;
		m_bWireframe = false;
	}

	// Textures
	TextureProperty m_pTexAlbedo;
	TextureProperty m_pTexEmission;
	TextureProperty m_pTexNormal;
	TextureProperty m_pTexMask;		// R-Roughness, G-Metallic, B-Occlusion, A-Height 

	// Material properties
	glm::vec4	m_colAlbedo;
	glm::vec4	m_colEmission;
	float		m_fRoughness;
	float		m_fMetallic;
	float		m_fOcclusion;
	float		m_fHeight;
	bool		m_bWireframe;
};

