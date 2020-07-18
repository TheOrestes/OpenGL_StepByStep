
#include "GLSLShader.h"
#include "../UI/UIManager.h"
#include <iostream>
#include <fstream>

//////////////////////////////////////////////////////////////////////////
GLSLShader::GLSLShader(const std::string& vertShader, const std::string& fragShader)
{
	shaderID = LoadShader(vertShader, fragShader);
}

//////////////////////////////////////////////////////////////////////////
GLSLShader::GLSLShader(const std::string& computeShader)
{
	shaderID = LoadShader(computeShader);
}

//////////////////////////////////////////////////////////////////////////
GLSLShader::~GLSLShader()
{
	glDeleteProgram(shaderID);
}

//////////////////////////////////////////////////////////////////////////
GLuint GLSLShader::LoadShader(const std::string& vShader, const std::string& fShader)
{
	/// Read vertex shader code from the file...
	std::string vertexShaderSrc;
	std::ifstream vsStream(vShader.c_str(), std::ios::in);

	if(vsStream.is_open())
	{
		std::string line;
		while(std::getline(vsStream, line))
		{
			vertexShaderSrc += "\n" + line;
		}

		vsStream.close();
	}

	/// Read pixel shader code from the file...
	std::string pixelShaderSrc;
	std::ifstream psStream(fShader.c_str(), std::ios::in);

	if(psStream.is_open())
	{
		std::string line;
		while(std::getline(psStream, line))
		{
			pixelShaderSrc += "\n" + line;
		}

		psStream.close();
	}

	// create shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	/// Compile Vertex shader...
	const char* ptrVertexShaderSrc = vertexShaderSrc.c_str();
	glShaderSource(vertexShaderID, 1, &ptrVertexShaderSrc, NULL);
	glCompileShader(vertexShaderID);

	// Check for shader compilation errors...
	if(!IsShaderCompiled(vertexShaderID, vShader))
		return 0;

	/// Compile fragment shader...
	const char* ptrFragShaderSrc = pixelShaderSrc.c_str();
	glShaderSource(fragmentShaderID, 1, &ptrFragShaderSrc, NULL);
	glCompileShader(fragmentShaderID);

	// Check for shader compilation errors...
	if(!IsShaderCompiled(fragmentShaderID, fShader))
		return 0;

	// Create shader program
	GLuint shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShaderID);
	glAttachShader(shaderProgramID, fragmentShaderID);
	glLinkProgram(shaderProgramID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return shaderProgramID;
}

//////////////////////////////////////////////////////////////////////////
GLuint GLSLShader::LoadShader(const std::string& cShader)
{
	/// Read vertex shader code from the file...
	std::string computeShaderSrc;
	std::ifstream csStream(cShader.c_str(), std::ios::in);

	if (csStream.is_open())
	{
		std::string line;
		while (std::getline(csStream, line))
		{
			computeShaderSrc += "\n" + line;
		}

		csStream.close();
	}

	// create shaders
	GLuint computeShaderID = glCreateShader(GL_COMPUTE_SHADER);
	
	/// Compile Vertex shader...
	const char* ptrComputeShaderSrc = computeShaderSrc.c_str();
	glShaderSource(computeShaderID, 1, &ptrComputeShaderSrc, NULL);
	glCompileShader(computeShaderID);

	// Check for shader compilation errors...
	if (!IsShaderCompiled(computeShaderID, cShader))
		return 0;

	// Create shader program
	GLuint shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, computeShaderID);
	glLinkProgram(shaderProgramID);

	glDeleteShader(computeShaderID);

	return shaderProgramID;
}

//////////////////////////////////////////////////////////////////////////
bool GLSLShader::IsShaderCompiled(GLuint shaderID, const std::string& name)
{
	GLint result = GL_FALSE;
	int infoLogLength;
	char infoLog[2048];

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

	if(!result)
	{
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, infoLog);
		std::string error = "Error compiling shader : " + name + ":" + infoLog;
		UIManager::getInstance().WriteToConsole(LOG_ERROR, "GLSLShader", error);
		std::cout << "Error compiling shader :" << name.c_str() << " : " << infoLog;
		return false;
	}
	else
	{
		std::string info = name + " compiled!";
		UIManager::getInstance().WriteToConsole(LOG_DEBUG, "GLSLShader", info);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void GLSLShader::Use()
{
	glUseProgram(shaderID);
}