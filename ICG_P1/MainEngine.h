#pragma once
#include "SceneManager.h"
#include "GraphicSettings.h"
#include "Game.h"
#include "Light.h"

#define MAX_LIGHTS 8
#define NUM_LIGHTS 2

class MainEngine
{
    public:
        // Variables
        static GLuint _nullTexture;
        static Game * _game;
        static SceneManager * _gameObjectManager;
        static bool _collisionsActive;
        static bool _enableBumpMapping;
        static bool _enableShadows;
        static map <std::string, GLuint> shaders;
        static sf::Clock frameClock;
        static sf::Clock gameClock;
        static sf::RenderWindow * _mainWindow;
        static vector<Light *> light;
        // Methods
        static float GetFrameTime();
        static void LoadShaders();
        static void CreateNullTexture(int width, int height);
};
