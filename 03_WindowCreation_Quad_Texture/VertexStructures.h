
#pragma once

#include "GL\glew.h"
#include <string>
#include "glm/glm.hpp"


//////////////////////////////////////////////////////////////////////////////////////////
struct VertexP
{
	VertexP() : position(0.0f){}
	VertexP(glm::vec3 _p) : position(_p){}

	glm::vec3 position;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct VertexPT
{
	VertexPT() : 
		position(0.0f),
		texcoord(0.0f, 0.0f){}

	VertexPT(const glm::vec3& _p, const glm::vec2& _uv) : 
		position(_p),
		texcoord(_uv) {}

	glm::vec3	position;
	glm::vec2	texcoord;
};











