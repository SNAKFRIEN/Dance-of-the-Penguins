#pragma once

#include "Shader.h"

class Cube
{
public:
	Cube();
	void Draw() const;
private:
	unsigned int vao = 0;
	unsigned int vbo = 0;
	Shader shader;
	unsigned int texture0 = 0;
	unsigned int texture1 = 0;
};