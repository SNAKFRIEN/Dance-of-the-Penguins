#pragma once

class Window;
class SaveFile;

//Used for post processing
class ScreenQuad
{
public:
	ScreenQuad(const Window& window, const SaveFile& settings);

	void StartFrame();
	void EndFrame();
	void Draw();
	void UpdateDimensions();

	unsigned int GetTexture() const;
private:
	//Geometry
	unsigned int vao = 0;
	unsigned int vbo;
	unsigned int ebo;

	//fbo
	unsigned int fbo;
	unsigned int msFbo;

	//rbo
	unsigned int msRbo;

	//Texture
	unsigned int texture = 0;
	unsigned int msTexture = 0;

	//Settings
	const Window& window;
	const SaveFile& settings;
};