#pragma once

#include "Shader.h"

class Triangle
{
public:
	Triangle();
	void Draw() const;
private:
	unsigned int vao = 0;
	unsigned int vbo = 0;
	unsigned int ebo = 0;
	Shader shader;
	unsigned int texture0 = 0;
	unsigned int texture1 = 0;
};