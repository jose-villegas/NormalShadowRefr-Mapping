#include "stdafx.h"
#include "Light.h"
#include "DefaultCamera.h"

Light::Light(void)
{
    active = true;
    intensity = 100;
    lightType = 1;
    position[0] = 0.0; position[1] = 80.0; position[2] = -100.0; position[3] = 1.0;
    direction[0] = 0.0f; direction[1] = 0.0f; direction[2] = -1.0f;
    ambient[0] = 0.2; ambient[1] = 0.2; ambient[2] = 0.2; ambient[3] = 1.0;
    diffuse[0] = 1.0; diffuse[1] = 1.0; diffuse[2] = 1.0; diffuse[3] = 1.0;
    specular[0] = 1.0; specular[1] = 1.0; specular[2] = 1.0; specular[3] = 1.0;
    const_att = 2.0;
    linear_att = 1.0;
    quad_att = 0.5;
    cutoff = 180;
    spot_exp = 0;
}

Light::~Light(void)
{
}

void Light::Enable(GLenum lightN)
{
    glEnable(lightN);
    float ambient0[4], diffuse0[4], specular0[4];

    for (int i = 0; i < 4; i++)
    {
        ambient0[i] = ambient[i] * intensity;
        diffuse0[i] = diffuse[i] * intensity;
        specular0[i] = specular[i] * intensity;
    }

    position[3] = lightType == 0 ? 0.0f : 1.0f;
    glLightfv(lightN, GL_POSITION, position);
    glLightfv(lightN, GL_AMBIENT, ambient0);
    glLightfv(lightN, GL_DIFFUSE, diffuse0);
    glLightfv(lightN, GL_SPECULAR, specular0);
    glLightf(lightN, GL_CONSTANT_ATTENUATION, const_att);
    glLightf(lightN, GL_LINEAR_ATTENUATION, linear_att);
    glLightf(lightN, GL_QUADRATIC_ATTENUATION, quad_att);
    glLightfv(lightN, GL_SPOT_DIRECTION, direction);

    if (lightType == 2)
    {
        if (cutoff == 180.0f) { cutoff = 90.0f; }

        glLightf(lightN, GL_SPOT_CUTOFF, cutoff);
        glLightf(lightN, GL_SPOT_EXPONENT, spot_exp);
    }
    else
    {
        cutoff = 180;
        glLightf(lightN, GL_SPOT_CUTOFF, cutoff);
        glLightf(lightN, GL_SPOT_EXPONENT, 0);
    }
}

void Light::Disable(GLenum lightN)
{
    glDisable(lightN);
    float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(lightN, GL_AMBIENT, black);
    glLightfv(lightN, GL_DIFFUSE, black);
    glLightfv(lightN, GL_SPECULAR, black);
}

void Light::Draw(GLenum lightN)
{
    if (active)
    {
        Enable(lightN);
    }
    else
    {
        Disable(lightN);
    }
}

void Light::Update(float elapsedTime, sf::Event input)
{
}

void Light::SetPosition(float pos[4])
{
    for (int i = 0; i < 4; i++)
    {
        position[i] = pos[i];
    }
}

void Light::SetPosition(float x, float y, float z)
{
    position[0] = x;
    position[1] = y;
    position[2] = z;
}

void Light::SetDiffuse(float diff[4])
{
    for (int i = 0; i < 4; i++)
    {
        diffuse[i] = diff[i];
    }
}

void Light::SetDiffuse(float x, float y, float z, float w)
{
    diffuse[0] = x;
    diffuse[1] = y;
    diffuse[2] = z;
    diffuse[3] = w;
}