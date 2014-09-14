#include "stdafx.h"
#include "DefaultCamera.h"

void DefaultCamera::UpdateCamera()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt
		(
		position.x, position.y, position.z,
		lookat.x, lookat.y, lookat.z,
		upVector.x, upVector.y, upVector.z
		);
}

void DefaultCamera::LookAt(sf::Vector3f model)
{
	lookat = model;
}

void DefaultCamera::SetPosition(sf::Vector3f pos)
{
	position = pos;
}

sf::Vector3f DefaultCamera::direction = sf::Vector3f(0, 0, -1);
sf::Vector3f DefaultCamera::position = sf::Vector3f(0, 0, 0);
sf::Vector3f DefaultCamera::lookat = sf::Vector3f(0, 0, -1);
sf::Vector3f DefaultCamera::upVector = sf::Vector3f(0, 1, 0);