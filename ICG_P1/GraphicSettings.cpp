#include "stdafx.h"
#include "GraphicSettings.h"


GraphicSettings::GraphicSettings(void)
{
}


GraphicSettings::~GraphicSettings(void)
{
}

void GraphicSettings::setAnistrophy( float value )
{
    float g_maxAnisotrophy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &g_maxAnisotrophy);

    if (value > g_maxAnisotrophy)
    {
        anisotrophyValue = g_maxAnisotrophy;
    }

    else
    {
        anisotrophyValue = value;
    }
}

float GraphicSettings::lifetimeProjectiles = 1.5f;
float GraphicSettings::anisotrophyValue = 1.0f;
float GraphicSettings::renderHeight = 800;
float GraphicSettings::renderWidth = 600;
float GraphicSettings::distanceDebris = 1500;
sf::ContextSettings GraphicSettings::settings;