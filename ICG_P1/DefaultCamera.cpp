#include "stdafx.h"
#include "DefaultCamera.h"

void DefaultCamera::UpdateCamera()
{
    gluLookAt
    (
        position.x, position.y, position.z,
        lookat.x, lookat.y, lookat.z,
        upVector.x, upVector.y, upVector.z
    );
}

void DefaultCamera::LookAt( sf::Vector3f model )
{
    lookat = model;
    upVector = sf::Vector3f(0.0f, 1.0f, 0.0f);
}

void DefaultCamera::SetPosition( sf::Vector3f pos )
{
    position = pos;
    upVector = sf::Vector3f(0.0f, 1.0f, 0.0f);
}

sf::Vector3f DefaultCamera::position;
sf::Vector3f DefaultCamera::lookat;
sf::Vector3f DefaultCamera::upVector;