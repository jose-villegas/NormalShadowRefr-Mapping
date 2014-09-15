#include "stdafx.h"
#include "Game.h"
#include "GraphicSettings.h"
#include "DefaultCamera.h"
#include "MainEngine.h"
#include <stddef.h>

Game::Game(void)
{
    selectedObjectName = "None";
    this->_gameState = Uninitialized;
}

Game::~Game(void)
{
}

void Game::Start(void)
{
    srand(time(NULL));

    if (_gameState != Uninitialized)
    {
        return;
    }

    // Start Glew
    InitGlew();
    // Settings
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    // MainWindow Settings
    InitMainWindow(desktop);
    // OpenGL Parameters
    InitOpenGL(desktop);
    // Load OBJ Models
    LoadModels();
    // Setting startup state
    _gameState = Game::Playing;
    MainEngineInit();
    // Load Shaders
    MainEngine::LoadShaders();
    // Setup Anttweakbar
    InitUI();

    while (!IsExiting())
    {
        GameLoop();
    }

    TwTerminate();
    _mainWindow.close();
}

bool Game::IsExiting()
{
    if (_gameState == Game::Exiting)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Game::GameLoop()
{
    sf::Event currentEvent;

    while (_mainWindow.pollEvent(currentEvent))
    {
        // Update Objects Logic
        int handled = TwEventSFML(&currentEvent, 1, 6);

        if (!handled)
        {
            if (currentEvent.type == sf::Event::Resized)
            {
                glViewport(0, 0, currentEvent.size.width, currentEvent.size.height);
                GraphicSettings::setRenderWidth(currentEvent.size.width);
                GraphicSettings::setRenderHeight(currentEvent.size.height);
            }

            if (currentEvent.type == sf::Event::Closed)
            {
                _gameState = Game::Exiting;
            }

            if (currentEvent.type == sf::Event::KeyReleased && currentEvent.key.code == sf::Keyboard::F)
            {
                for (int i = 0; i < 6; i++)
                {
                    _scene.GetAt(i)->EnableProgrammablePipeline(false);
                }
            }

            if (currentEvent.type == sf::Event::KeyReleased && currentEvent.key.code == sf::Keyboard::G)
            {
                for (int i = 0; i < 6; i++)
                {
                    _scene.GetAt(i)->EnableProgrammablePipeline(true);
                }
            }

            if (currentEvent.type == sf::Event::MouseButtonReleased && currentEvent.mouseButton.button == sf::Mouse::Left)
            {
                DrawSelectionMode();
                unsigned char pixel[3];
                GLint viewport[4];
                glGetIntegerv(GL_VIEWPORT, viewport);
                glReadPixels(currentEvent.mouseButton.x, viewport[3] - currentEvent.mouseButton.y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE,
                             pixel);

                if ((int)pixel[0] >= 250 && (int)pixel[0] <= 255)
                {
                    selectedObjectName = _scene.GetAt(255 - (int)pixel[0])->GetManagerName();
                    selectedObject = _scene.GetAt(255 - (int)pixel[0]);
                }

                glEnable(GL_TEXTURE_2D);
                glEnable(GL_LIGHTING);
                glColor3f(1.0f, 1.0f, 1.0f);
            }

            _scene.UpdateAll(currentEvent);
        }

        for (int i = 0; i < MainEngine::light.size(); i++)
        {
            // Update Light Positions
            MainEngine::light[i]->SetPosition(MainEngine::light[i]->position[0], MainEngine::light[i]->position[1],
                                              MainEngine::light[i]->position[2]);
        }
    }

    switch (_gameState)
    {
        case Game::Playing:
            {
                _mainWindow.clear();
                // Clear the depth buffer
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                // Apply some transformations
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                // Update Camera
                DefaultCamera::UpdateCamera();
                // Render Scene
                _scene.Draw();
                // Draw Lights
                DrawLightSpheres();
                // Objects Picking
                //DrawSelectionMode();
                // Draw Anttweakbar
                TwDraw();
                // Display Changes
                _mainWindow.display();
            }
            break;
    }
}

vector <VisibleGameObject *> indicator;
void Game::DrawLightSpheres()
{
    if (indicator.empty())
    {
        VisibleGameObject * vgo = new VisibleGameObject("Models/sphere.obj");

        for (int i = 0; i < MainEngine::light.size(); i++)
        {
            indicator.push_back(new VisibleGameObject(*vgo));
        }
    }

    glDisable(GL_LIGHTING);

    for (int i = 0; i < MainEngine::light.size(); i++)
    {
        MainEngine::light[i]->Disable(GL_LIGHT0 + i);
        indicator[i]->EnableProgrammablePipeline(false);
        glUseProgram(0);
        indicator[i]->SetPosition(MainEngine::light[i]->position[0], MainEngine::light[i]->position[1],
                                  MainEngine::light[i]->position[2]);
        indicator[i]->Draw();
    }

    glEnable(GL_LIGHTING);

    // Draw Lights
    for (int i = 0; i < MainEngine::light.size(); i++)
    {
        MainEngine::light[i]->Draw(GL_LIGHT0 + i);
    }
}

void Game::InitUI()
{
    // Initialize AntTweakBar
    TwInit(TW_OPENGL, NULL);
    // Tell the window size to AntTweakBar
    TwWindowSize(_mainWindow.getSize().x , _mainWindow.getSize().y);
    // Create a tweak bar
    TwBar * bar = TwNewBar("Luces");
    TwDefine(" GLOBAL help='mensaje de ayuda #todo' "); // Message added to the help bar.
    // Change bar position
    int barPos[2] = {7, 7};
    TwSetParam(bar, NULL, "position", TW_PARAM_INT32, 2, &barPos);
    // Add 'speedDir' to 'bar': this is a modifiable variable of type TW_TYPE_DIR3F. Just displaying the arrow widget
    struct Point { float X, Y, Z; };
    TwStructMember pointMembers[] =
    {
        { "X", TW_TYPE_FLOAT, offsetof(Point, X), " Step=0.1 " },
        { "Y", TW_TYPE_FLOAT, offsetof(Point, Y), " Step=0.1 " },
        { "Z", TW_TYPE_FLOAT, offsetof(Point, Z), " Step=0.1 " }
    };
    TwType pointType = TwDefineStruct("POINT", pointMembers, 3, sizeof(Point), NULL, NULL);
    // Defining new enum type
    TwType lightType = TwDefineEnumFromString("Light Type", "Directional,Point,Spot");

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        Light * b = new Light();
        MainEngine::light.push_back(b);
        TwAddVarRW(bar, ("LightType" + std::to_string(i)).c_str(), lightType, &MainEngine::light.back()->lightType,
                   (" Group= 'Luz " + std::to_string(i) + "' Label='Light Type' ").c_str());
        TwAddVarRW(bar, ("Active" + std::to_string(i)).c_str(), TW_TYPE_BOOLCPP, &MainEngine::light.back()->active,
                   (" Group= 'Luz " + std::to_string(i) + "' Label='Active' ").c_str());
        TwAddVarRW(bar, ("Position" + std::to_string(i)).c_str(), pointType, &MainEngine::light.back()->position,
                   (" Group='Luz " + std::to_string(i) + "' Label='Position' ").c_str());
        TwAddVarRW(bar, ("Direction" + std::to_string(i)).c_str(), TW_TYPE_DIR3F, &MainEngine::light.back()->direction,
                   (" Group='Luz " + std::to_string(i) + "' Label='Direction' ").c_str());
        TwAddVarRW(bar, ("Ambient" + std::to_string(i)).c_str(), TW_TYPE_COLOR3F, &MainEngine::light.back()->ambient,
                   (" Group='Luz " + std::to_string(i) + "' colorMode=rgb Label='Ambient' ").c_str());
        TwAddVarRW(bar, ("Diffuse" + std::to_string(i)).c_str(), TW_TYPE_COLOR3F, &MainEngine::light.back()->diffuse,
                   (" Group='Luz " + std::to_string(i) + "' colorMode=rgb Label='Diffuse' ").c_str());
        TwAddVarRW(bar, ("Specular" + std::to_string(i)).c_str(), TW_TYPE_COLOR3F, &MainEngine::light.back()->specular,
                   (" Group='Luz " + std::to_string(i) + "' colorMode=rgb Label='Specular' ").c_str());
        TwAddVarRW(bar, ("Intensity" + std::to_string(i)).c_str(), TW_TYPE_FLOAT, &MainEngine::light.back()->intensity,
                   (" Group='Luz " + std::to_string(i) + "' Label='Intensity' step=0.1").c_str());
        TwAddButton(bar, NULL, NULL, NULL, ("label = 'Attenuation' group='Luz " + std::to_string(i) + "'").c_str());
        TwAddVarRW(bar, ("CONSTANT_ATTENUATION" + std::to_string(i)).c_str(), TW_TYPE_FLOAT,
                   &MainEngine::light.back()->const_att, (" Group='Luz " + std::to_string(i) + "' Label='   Constant'").c_str());
        TwAddVarRW(bar, ("LINEAR_ATTENUATIO" + std::to_string(i)).c_str(), TW_TYPE_FLOAT, &MainEngine::light.back()->linear_att,
                   (" Group='Luz " + std::to_string(i) + "' Label='   Linear'").c_str());
        TwAddVarRW(bar, ("QUADRATIC_ATTENUATION" + std::to_string(i)).c_str(), TW_TYPE_FLOAT,
                   &MainEngine::light.back()->quad_att, (" Group='Luz " + std::to_string(i) + "' Label='   Quadratic'").c_str());
        TwAddButton(bar, NULL, NULL, NULL, ("label='Spot Light' group='Luz " + std::to_string(i) + "'").c_str());
        TwAddVarRW(bar, ("Cutoff" + std::to_string(i)).c_str(), TW_TYPE_FLOAT, &MainEngine::light.back()->cutoff,
                   (" Group='Luz " + std::to_string(i) + "' Label='   Cutoff' min=0 max=90 ").c_str());
        TwAddVarRW(bar, ("Exponent" + std::to_string(i)).c_str(), TW_TYPE_FLOAT, &MainEngine::light.back()->spot_exp,
                   (" Group='Luz " + std::to_string(i) + "' Label='   Exponent' min=0 max=128").c_str());
    }

    TwBar * bar2 = TwNewBar("Seleccion");
    // Change bar position
    TwDefine(" Seleccion position='7 340' ");
    TwDefine(" Seleccion size='200 200' ");
    TwAddVarRW(bar2, "Objeto Actual", TW_TYPE_STDSTRING, &selectedObjectName, "");
    TwAddVarCB(bar2, "rotation", TW_TYPE_QUAT4F, SetRotationCB, GetRotationCB, this,
               " Label='Rotation' opened=true showval=true axisx=x axisy=y axisz=-z");
    TwBar * bar3 = TwNewBar("Opciones");
    // Change bar position
    TwDefine(" Opciones position='7 550' ");
    TwDefine(" Opciones size='200 250' ");
    TwAddVarRW(bar3, "Bump Mapping", TW_TYPE_BOOLCPP, &MainEngine::_enableBumpMapping, "");
    TwAddVarCB(bar3, "Camera Position", pointType, SetCameraCB, GetCameraCB, NULL, "opened=true");
    TwAddVarCB(bar3, "Camera Direction", TW_TYPE_DIR3F, SetCameraDirCB, GetCameraDirCB, NULL, "opened=true");
}

void Game::DrawSelectionMode()
{
    //for (int i = 0; i < _gameObjectManager.GetGameObjects().size(); i++)
    //{
    //    //glColor3f((255.0f - i) / 255.0f, (255.0f - i) / 255.0f, (255.0f - i) / 255.0f);
    //    _gameObjectManager.GetAt(i)->Draw();
    //}
}

void TW_CALL Game::GetRotationCB(void * value, void * clientData)
{
    float * a = static_cast<const Game *>(clientData)->selectedObject->GetObjectRotation();
    float * val = (float *)value;
    val[0] = a[0];
    val[1] = a[1];
    val[2] = a[2];
    val[3] = a[3];
}

void TW_CALL Game::SetRotationCB(const void * value, void * clientData)
{
    float * a = (float *)value;
    ((Game *)clientData)->selectedObject->RotateObject(a);
}

void TW_CALL Game::GetCameraCB(void * value, void * clientData)
{
    float * val = (float *)value;
    val[0] = DefaultCamera::position.x;
    val[1] = DefaultCamera::position.y;
    val[2] = DefaultCamera::position.z;
}

void TW_CALL Game::SetCameraCB(const void * value, void * clientData)
{
    float * a = (float *)value;
    glm::vec3 lightDir = DefaultCamera::direction;
    glm::vec3 lightPos = glm::vec3(a[0], a[1], a[2]);
    glm::vec3 invDir = glm::vec3(-lightDir.x, -lightDir.y, -lightDir.z);
    DefaultCamera::SetPosition(lightPos);
    DefaultCamera::LookAt(glm::vec3(lightPos.x - invDir.x, lightPos.y - invDir.y, lightPos.z - invDir.z));
}

void TW_CALL Game::GetCameraDirCB(void * value, void * clientData)
{
    float * val = (float *)value;
    val[0] = DefaultCamera::direction.x;
    val[1] = DefaultCamera::direction.y;
    val[2] = DefaultCamera::direction.z;
}

void TW_CALL Game::SetCameraDirCB(const void * value, void * clientData)
{
    float * a = (float *)value;
    glm::vec3 lightPos = DefaultCamera::position;
    glm::vec3 invDir = glm::vec3(-a[0], -a[1], -a[2]);
    DefaultCamera::direction = glm::vec3(a[0], a[1], a[2]);
    DefaultCamera::SetPosition(lightPos);
    DefaultCamera::LookAt(glm::vec3(lightPos.x - invDir.x, lightPos.y - invDir.y, lightPos.z - invDir.z));
}

void Game::InitOpenGL(sf::VideoMode &desktop)
{
    // Configure the viewport (the same size as the window)
    glViewport(0, 0, _mainWindow.getSize().x, _mainWindow.getSize().y);
    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, desktop.width / desktop.height, 1.f, 2000);
    //Texturas
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Game::MainEngineInit()
{
    // Make some data avaible through MainEngine
    MainEngine::_game = this;
    MainEngine::_gameObjectManager = &_scene;
    MainEngine::_mainWindow = &_mainWindow;
    // Create Textures and Frame Buffers
    MainEngine::CreateNullTexture(2, 2);
    MainEngine::CreateShadowFBO(_mainWindow.getSize().x, _mainWindow.getSize().y);
}

void Game::InitGlew()
{
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile

    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return;
    }
}

void Game::InitMainWindow(sf::VideoMode &desktop)
{
    sf::ContextSettings Settings;
    Settings.depthBits = desktop.bitsPerPixel; // Request a 32 bits depth buffer
    Settings.antialiasingLevel = 8; // Anti aliasing
    GraphicSettings::setAnistrophy(16.0f); // Filtro anistropico
    GraphicSettings::setSettings(Settings);
    // Create Window
    _mainWindow.create(sf::VideoMode(desktop.width - 200, desktop.height - 200), "Ping!", sf::Style::Close, Settings);
    _mainWindow.setActive();
    _mainWindow.setFramerateLimit(60);
    GraphicSettings::setRenderHeight(desktop.height);
    GraphicSettings::setRenderWidth(desktop.width);
}

void Game::LoadModels()
{
    VisibleGameObject * bunny = new VisibleGameObject("Models/Cube/cube.obj");
    //bunny->SetPosition(-50.0f, -50.0f, -100.0f);
    bunny->SetPosition(50, -25, -100.0f);
    bunny->Scale(40);
    VisibleGameObject * suzanne = new VisibleGameObject("Models/teapot.obj");
    suzanne->SetPosition(0.0f, -50.0f, -100.0f);
    suzanne->Scale(40);
    VisibleGameObject * teapot = new VisibleGameObject("Models/floor/floor.obj");
    teapot->SetPosition(0.0f, -60.0f, -150.0f);
    teapot->Scale(300);
    _scene.Add(bunny->GetFilepath(), bunny);
    _scene.Add(suzanne->GetFilepath(), suzanne);
    _scene.Add(teapot->GetFilepath(), teapot);
    selectedObjectName = _scene.GetAt(0)->GetManagerName();
    selectedObject = _scene.GetAt(0);
}
