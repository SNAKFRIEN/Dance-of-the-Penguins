#include "IceRink.h"

IceRink::IceRink()
	:
	stadiumModel("Stadium.gltf", transform, "SmoothShader.vert", "SmoothShader.frag"),
	iceModel("Ice.gltf", transform)
{
	transform = glm::mat4(1.0f);
}

void IceRink::Draw(Camera& camera)
{
	stadiumModel.Draw(camera);
	iceModel.Draw(camera);
}
