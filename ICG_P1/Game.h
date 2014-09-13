#pragma once
#include "SceneManager.h"

class Game
{
    public:
        Game(void);
        ~Game(void);
        void Start();

    private:

        sf::Texture background;
        sf::RenderWindow _mainWindow;
        SceneManager _scene;

        enum GameState { Uninitialized, Paused, Playing, Exiting};
        GameState _gameState;

        string selectedObjectName;
        VisibleGameObject * selectedObject;
        float ** ambient, * * diffuse, * * specular, * shininess;

        bool IsExiting();
        void GameLoop();

		void UpdateCamera( sf::Event &currentEvent );

		void ShowSplashScreen();
        void ShowMenu();
        void DrawLightSpheres();
        void DrawSelectionMode();
        void InitUI();

        static void TW_CALL GetRotationCB(void * value, void * clientData);
        static void TW_CALL SetRotationCB(const void * value, void * clientData);
};

