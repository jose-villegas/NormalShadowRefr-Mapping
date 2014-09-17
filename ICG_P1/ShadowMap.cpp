#include "stdafx.h"
#include "ShadowMap.h"
#include "MainEngine.h"
#include "glm\gtc\matrix_transform.hpp"


GLuint ShadowMap::_depthMap = 0;

GLuint ShadowMap::_fbo = 0;

void ShadowMap::Delete()
{
    if (_fbo != 0)
    {
        glDeleteFramebuffers(1, &_fbo);
    }

    if (_depthMap != 0)
    {
        glDeleteTextures(1, &_depthMap);
    }
}

bool ShadowMap::Create(unsigned int width, unsigned int height)
{
    size = glm::vec2(width, height);
    GLenum FBOstatus;
    // create a framebuffer object
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    // Try to use a texture depth component
    glGenTextures(1, &_depthMap);
    glBindTexture(GL_TEXTURE_2D, _depthMap);
    // No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    // GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Remove artifact on the edges of the shadowmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // attach the texture to FBO depth attachment point
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthMap, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Instruct openGL that we won't bind a color texture with the currently bound FBO
    glDrawBuffer(GL_NONE);
    // check FBO status
    FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (FBOstatus != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");
    }

    // switch back to window-system-provided framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

glm::mat4 ShadowMap::CalculateDepthVP(Light * light)
{
    float aspectRatio = MainEngine::_mainWindow->getSize().x / MainEngine::_mainWindow->getSize().y;
    // Light Params
    glm::vec3 lightInvDir = glm::vec3(-light->direction[0], -light->direction[1], -light->direction[2]);
    glm::vec3 lightPos = glm::vec3(light->position[0], light->position[1], light->position[2]);
    // Matrices
    //glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos - lightInvDir, glm::vec3(0, 1, 0));
    //glm::mat4 depthProjectionMatrix = glm::perspective<float>(90.0f, aspectRatio, 1.0f, 2000.0f);
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-1000, 1000, -1000, 1000, 1, 2000);
    glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0, 0, -100), glm::vec3(0, 1, 0));
    // VP
    depthVP = depthProjectionMatrix * depthViewMatrix;
    return depthVP;
}

glm::mat4 ShadowMap::CalculateMVPMatrix(VisibleGameObject * model)
{
    glm::mat4 depthModel = glm::mat4(1);
    depthModel = glm::translate(depthModel, model->GetPosition());
    depthModel *= model->GetRotationMatrix();
    depthModel *= glm::scale(glm::mat4(1), glm::vec3(model->GetScaleFactor()));
    depthMVP = depthVP * depthModel;
    return depthMVP;
}

glm::mat4 ShadowMap::CalculateBiasMVPMatrix(VisibleGameObject * model)
{
    glm::mat4 depthModel = glm::mat4(1);
    depthModel = glm::translate(depthModel, model->GetPosition());
    depthModel *= model->GetRotationMatrix();
    depthModel *= glm::scale(glm::mat4(1), glm::vec3(model->GetScaleFactor()));
    depthMVP = depthVP * depthModel;
    depthBiasMVP = biasMatrix * depthMVP;
    return depthBiasMVP;
}

void ShadowMap::DepthTestingMode()
{
    depthTesting = true;
    mapping = false;
}

void ShadowMap::MappingMode()
{
    depthTesting = false;
    mapping = true;
}

glm::vec2 ShadowMap::size;

// This matrix stays the same between models
glm::mat4 ShadowMap::depthVP;

// This matrix changes between models
glm::mat4 ShadowMap::depthBiasMVP;
glm::mat4 ShadowMap::depthMVP;

bool ShadowMap::mapping = false;

bool ShadowMap::depthTesting = false;

glm::mat4 ShadowMap::biasMatrix = glm::mat4(
                                      0.5, 0.0, 0.0, 0.0,
                                      0.0, 0.5, 0.0, 0.0,
                                      0.0, 0.0, 0.5, 0.0,
                                      0.5, 0.5, 0.5, 1.0);
