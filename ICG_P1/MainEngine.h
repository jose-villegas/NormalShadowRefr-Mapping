#pragma once
#include "SceneManager.h"
#include "GraphicSettings.h"
#include "Game.h"
#include "Light.h"

#define NUM_LIGHTS 2
#define MAX_LIGHTS 8

class ShadowMapFBO
{
    public:
        ShadowMapFBO();
        ~ShadowMapFBO();
        bool Load(unsigned int WindowWidth, unsigned int WindowHeight);
        void BindForWriting();
        void BindForReading(GLenum TextureUnit);
        GLuint GetFbo() const { return m_fbo; }
        GLuint GetShadowMap() const { return m_shadowMap; }

    private:
        GLuint m_fbo;
        GLuint m_shadowMap;
};

class MainEngine
{
    public:
        // Variables
        static bool _enableShadows;
        static bool _collisionsActive;
        static bool _enableBumpMapping;
        static SceneManager * _gameObjectManager;
        static Game * _game;
        static sf::RenderWindow * _mainWindow;
        static ShadowMapFBO m_shadowMapFBO;
        static sf::Clock frameClock;
        static sf::Clock gameClock;
        static GLuint _nullTexture;
        static vector<Light *> light;
        static map <std::string, GLuint> shaders;
        // Methods
        static float GetFrameTime();
        static void LoadShaders();
        static void CreateNullTexture(int width, int height);
        static void CreateShadowFBO(int width, int heigh);
};
