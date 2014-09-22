#include "stdafx.h"
#include "MainEngine.h"
#include <fstream>

float MainEngine::GetFrameTime()
{
    return frameClock.restart().asSeconds();
}

GLuint BindAndCompileShader(std::string filepath, int shaderType)
{
    GLuint rtrn = glCreateShader(shaderType);
    const char * a;
    ifstream ifs(filepath.c_str());

    if (ifs)
    {
        string shaderSource((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        a = shaderSource.c_str();
        glShaderSource(rtrn, 1, &a, NULL);
        glCompileShader(rtrn);
    }

    return rtrn;
}

GLuint LoadShaderFromFile(std::string vertexFile, std::string pixelFile)
{
    GLuint program_object = glCreateProgram();;
    GLuint vertexShader = BindAndCompileShader(vertexFile, GL_VERTEX_SHADER);
    GLuint fragmentShader = BindAndCompileShader(pixelFile, GL_FRAGMENT_SHADER);
    // Attach Shaders to Program
    glAttachShader(program_object, vertexShader);
    glAttachShader(program_object, fragmentShader);
    // Link Program
    glLinkProgram(program_object);
    return program_object;
}

void MainEngine::LoadShaders()
{
    shaders["Picking"] = LoadShaderFromFile("picking.vert", "picking.frag");
    shaders["Depth"] = LoadShaderFromFile("depth.vert", "depth.frag");
    shaders["MainShader"] = LoadShaderFromFile("main_shader.vert", "main_shader.frag");
}

void MainEngine::CreateNullTexture(int width, int height)
{
    // Create an empty white texture. This texture is applied to OBJ models
    // that don't have any texture maps. This trick allows the same shader to
    // be used to draw the OBJ model with and without textures applied.
    int pitch = ((width * 32 + 31) & ~31) >> 3; // align to 4-byte boundaries
    std::vector<GLubyte> pixels(pitch * height, 255);
    glGenTextures(1, &_nullTexture);
    glBindTexture(GL_TEXTURE_2D, _nullTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, &pixels[0]);
}

bool MainEngine::_enableBumpMapping = true;

bool MainEngine::_enableShadows = true;

GLuint MainEngine::_nullTexture;

vector<Light *> MainEngine::light;

map <std::string, GLuint> MainEngine::shaders;

sf::RenderWindow * MainEngine::_mainWindow;

sf::Clock MainEngine::gameClock;

SceneManager * MainEngine::_gameObjectManager;

Game * MainEngine::_game;

sf::Clock MainEngine::frameClock;

bool MainEngine::_collisionsActive = true;
