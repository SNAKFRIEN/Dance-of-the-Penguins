#include "IceRink.h"

#include "glm/gtc/matrix_transform.hpp"
#include "json.hpp"

IceRink::IceRink(bool initModels)
{
	if (initModels)
	{
		InitModels();
	}
	transform = glm::mat4(1.0f);
	Reset();
}

void IceRink::Draw(Camera& camera, Input& input)
{
	if (!input.IsPressed(70))
	{
		stadiumModel->AddToRenderQueue(camera);
		iceModel->AddToRenderQueue(camera);
	}

	for (Model& s : seats)
	{
		s.AddToRenderQueue(camera);
	}
}

void IceRink::Reset()
{
	//Reset ice location
	iceTransform = glm::translate(glm::mat4(1.0f), defaultIcePos);	//Make sure that hole in ice is off screen
}

float IceRink::GetRight() const
{
	return right;
}

float IceRink::GetTop() const
{
	return top;
}

float IceRink::GetCornerRadius() const
{
	return cornerRadius;
}

void IceRink::SetIcePos(glm::vec3 newPos)
{
	iceTransform = glm::translate(glm::mat4(1.0f), newPos);
}

void IceRink::InitModels()
{
	stadiumModel = std::make_unique<Model>("Stadium.gltf", transform, "SmoothShader.vert", "SmoothShader.frag");
	iceModel = std::make_unique<Model>("Ice.gltf", iceTransform);

	//Load seats
	//Gain access to the data
	std::ifstream file("Miscellaneous/AudiencePositions.json");
	nlohmann::json data;
	file >> data;
	auto nSeats = data.size() / 4;
	seatTransforms.reserve(nSeats);
	seats.reserve(nSeats);
	for (int i = 0; i < data.size(); i += 4)
	{
		glm::mat4 temp(1.0f);
		float angle = (float)data[i + 3];
		glm::vec3 pos = glm::vec3((float)data[i], (float)data[i + 1], (float)data[i + 2]);

		temp = glm::translate(temp, pos);
		temp = glm::rotate(temp, angle, glm::vec3(0.0f, -1.0f, 0.0f));

		seatTransforms.push_back(temp);
		seats.emplace_back("Seat.gltf", seatTransforms[seatTransforms.size() - 1], "SmoothShader.vert", "SmoothShader.frag");
	}
}
