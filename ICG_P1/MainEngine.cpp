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
    shaders["Phong-Shading"] = LoadShaderFromFile("phong_sh.vertex", "phong_sh.pixel");
    shaders["Blinn-Phong"] = LoadShaderFromFile("blinnphong.vertex", "blinnphong.pixel");
    shaders["Cook-Torrance"] = LoadShaderFromFile("cook_torrance.vertex", "cook_torrance.pixel");
    shaders["Normal-Mapping"] = LoadShaderFromFile("normal_mapping.vertex", "normal_mapping.pixel");
    shaders["Shadow-Mapping"] = LoadShaderFromFile("shadow_mapping.vertex", "shadow_mapping.pixel");
    shaders["MainShader"] = LoadShaderFromFile("main_shader.vertex", "main_shader.pixel");
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

void MainEngine::CreateShadowFBO(int width, int heigh)
{
    m_shadowMapFBO.Load(width, heigh);
}

bool MainEngine::_enableShader = true;

ShadowMapFBO MainEngine::m_shadowMapFBO;

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

ShadowMapFBO::ShadowMapFBO()
{
    m_fbo = 0;
    m_shadowMap = 0;
}

ShadowMapFBO::~ShadowMapFBO()
{
    if (m_fbo != 0)
    {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_shadowMap != 0)
    {
        glDeleteTextures(1, &m_shadowMap);
    }
}

bool ShadowMapFBO::Load(unsigned int WindowWidth, unsigned int WindowHeight)
{
    GLenum FBOstatus;
    // Try to use a texture depth component
    glGenTextures(1, &m_shadowMap);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
    // GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Remove artifact on the edges of the shadowmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
                 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // create a framebuffer object
    glGenFramebuffersEXT(1, &m_fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    // Instruct openGL that we won't bind a color texture with the currently bound FBO
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    // attach the texture to FBO depth attachment point
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, m_shadowMap, 0);
    // check FBO status
    FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    if (FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");
    }

    // switch back to window-system-provided framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    return true;
}

void ShadowMapFBO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void ShadowMapFBO::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
}
