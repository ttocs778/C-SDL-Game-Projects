#pragma once
#include <GL/glew.h>
#include <string>
#include "Math.h"

class Shader
{
public:
	Shader();
	~Shader();

	// Load shader of the specified name, excluding
	// the .frag/.vert extension
	bool Load(const std::string& name);
	void Unload();
	// Set this as the active shader program
	void SetActive() const;
	// Sets a Matrix uniform
	void SetMatrixUniform(const char* name, const Matrix4& matrix) const;
	// Sets a Vector3 uniform
	void SetVectorUniform(const char* name, const Vector3& vector) const;
	void SetVector4Uniform(const char* name, const Vector4& vector) const;
	// Sets a float uniform
	void SetFloatUniform(const char* name, float value) const;
	// Sets an int uniform
	void SetIntUniform(const char* name, int value) const;

private:
	// Tries to compile the specified shader
	bool CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader);

	// Tests whether shader compiled successfully
	bool IsCompiled(GLuint shader);
	// Tests whether vertex/fragment programs link
	bool IsValidProgram() const;

	// Store the shader object IDs
	GLuint mVertexShader;
	GLuint mFragShader;
	GLuint mShaderProgram;
};
