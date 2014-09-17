#pragma once
#include "SceneManager.h"

class Game
{
    public:
        Game(void);
        ~Game(void);
        void Start();

        void LoadModels();

        void InitMainWindow(sf::VideoMode &desktop);

        void InitGlew();

        void MainEngineInit();

        void InitOpenGL(sf::VideoMode &desktop);

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

        void ShowSplashScreen();
        void ShowMenu();
        void DrawLightSpheres();
        void DrawSelectionMode();
        void InitUI();

        static void TW_CALL GetRotationCB(void * value, void * clientData);
        static void TW_CALL SetRotationCB(const void * value, void * clientData);

        static void TW_CALL GetPositionCB(void * value, void * clientData);
        static void TW_CALL SetPositionCB(const void * value, void * clientData);

        static void TW_CALL GetCameraCB(void * value, void * clientData);
        static void TW_CALL SetCameraCB(const void * value, void * clientData);

        static void TW_CALL GetCameraDirCB(void * value, void * clientData);
        static void TW_CALL SetCameraDirCB(const void * value, void * clientData);

        static void TW_CALL GetReflective(void * value, void * clientData);
        static void TW_CALL SetReflective(const void * value, void * clientData);

        static void TW_CALL GetRefractive(void * value, void * clientData);
        static void TW_CALL SetRefractive(const void * value, void * clientData);
};
