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

void DefaultCamera::LookAt(glm::vec3 model)
{
    lookat = model;
}

void DefaultCamera::SetPosition(glm::vec3 pos)
{
    position = pos;
}

glm::vec3 DefaultCamera::direction = glm::vec3(0, 0, -1);
glm::vec3 DefaultCamera::position = glm::vec3(0, 0, 0);
glm::vec3 DefaultCamera::lookat = glm::vec3(0, 0, -1);
glm::vec3 DefaultCamera::upVector = glm::vec3(0, 1, 0);