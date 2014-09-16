#include "stdafx.h"
#include "SceneManager.h"
#include "GraphicSettings.h"
#include "MainEngine.h"
#include "DefaultCamera.h"
#include "glm/gtc/matrix_transform.hpp"
#include <SDL_stdinc.h>
#include "ShadowMap.h"
#include "Reflection.h"

SceneManager::SceneManager(void)
{
}

SceneManager::~SceneManager()
{
    std::for_each(_gameObjects.begin(), _gameObjects.end(), GameObjectDeallocator());
}

void SceneManager::Add(std::string name, VisibleGameObject * gameObject)
{
    gameObject->SetManagerName(name);
    _gameObjects.push_back(std::pair<std::string, VisibleGameObject *>(name, gameObject));
}

void SceneManager::Remove(std::string name)
{
    vector<pair <std::string, VisibleGameObject *>>::iterator itr = find_if(_gameObjects.begin(), _gameObjects.end(),
            CompareFirst(name));

    if (itr != _gameObjects.end())
    {
        delete itr->second;
        _gameObjects.erase(itr);
    }
}

VisibleGameObject * SceneManager::Get(std::string name) const
{
    vector<pair <std::string, VisibleGameObject *>>::const_iterator itr = find_if(_gameObjects.begin(), _gameObjects.end(),
            CompareFirst(name));

    if (itr == _gameObjects.end())
    {
        return NULL;
    }

    return itr->second;
}

int SceneManager::GetObjectCount() const
{
    return _gameObjects.size();
}

void SceneManager::Draw()
{
    if (MainEngine::_enableShadows)
    {
        DrawShadowMapping();
    }
    else
    {
        glUseProgram(MainEngine::shaders["MainShader"]);
        glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "bEnableShadowMapping"), 0);
        DrawScene();
        glUseProgram(0);
    }

    // ---- Debug Show Depth Texture
    //glPushMatrix();
    //glOrtho(-int(MainEngine::_mainWindow->getSize().x) / 2, MainEngine::_mainWindow->getSize().x / 2,
    //        -int(MainEngine::_mainWindow->getSize().y) / 2, MainEngine::_mainWindow->getSize().y / 2, 1, 2000);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, MainEngine::m_shadowMapFBO.GetShadowMap());
    //glEnable(GL_TEXTURE_2D);
    //glTranslated(0, 0, - 1);
    //glBegin(GL_QUADS);
    //glTexCoord2d(0, 0); glVertex3f(0, 0, 0);
    //glTexCoord2d(1, 0); glVertex3f(MainEngine::_mainWindow->getSize().x / 2, 0, 0);
    //glTexCoord2d(1, 1); glVertex3f(MainEngine::_mainWindow->getSize().x / 2, MainEngine::_mainWindow->getSize().y / 2, 0);
    //glTexCoord2d(0, 1); glVertex3f(0, MainEngine::_mainWindow->getSize().y / 2, 0);
    //glEnd();
    //glDisable(GL_TEXTURE_2D);
    //glPopMatrix();
}

void SceneManager::UpdateAll(sf::Event input)
{
    for (int i = 0; i < _gameObjects.size(); i++)
    {
        _gameObjects[i].second->Update(MainEngine::GetFrameTime(), input);
    }
}

VisibleGameObject * SceneManager::GetAt(int index) const
{
    if (_gameObjects.empty() || index >= _gameObjects.size()) { return NULL; }

    return next(_gameObjects.begin(), index)->second;
}

void SceneManager::RemoveAt(int index)
{
    vector<pair <std::string, VisibleGameObject *>>::const_iterator results = next(_gameObjects.begin(), index);
    delete results->second;
    _gameObjects.erase(results);
}


void SceneManager::DrawScene()
{
    for (int i = 0; i < _gameObjects.size(); i++)
    {
        if (_gameObjects[i].second->GetEnableRender())
        {
            if (MainEngine::_enableShadows) { ComputeModelMatrix_ShadowMapping(i); }

            if (_gameObjects[i].second->GetIsReflective()) { DrawCubeMap(_gameObjects[i].second); }
            else { glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "bEnableReflection"), 0); }

            _gameObjects[i].second->Draw();
        }
    }
}

void SceneManager::DrawUsingFixedPipeline()
{
    for (int i = 0; i < _gameObjects.size(); i++)
    {
        if (_gameObjects[i].second->GetEnableRender())
        {
            if (MainEngine::_enableShadows) { ComputeModelMatrix_ShadowMapping(i); }

            _gameObjects[i].second->EnableProgrammablePipeline(false);
            _gameObjects[i].second->Draw();
            _gameObjects[i].second->EnableProgrammablePipeline(true);
        }
    }
}

void SceneManager::DrawShadowMapping()
{
    // Render from Light's POV
    ShadowMap::DepthTestingMode();
    glBindFramebuffer(GL_FRAMEBUFFER, ShadowMap::GetFrameBuffer());
    glViewport(0, 0, ShadowMap::GetSize().x, ShadowMap::GetSize().y);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    glUseProgram(MainEngine::shaders["Depth"]);
    ShadowMap::CalculateDepthVP(MainEngine::light[0]);
    DrawUsingFixedPipeline();
    glUseProgram(0);
    // Render from Camera's POV
    ShadowMap::MappingMode();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, MainEngine::_mainWindow->getSize().x, MainEngine::_mainWindow->getSize().y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glUseProgram(MainEngine::shaders["MainShader"]);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, ShadowMap::GetDepthMap());
    glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "shadowMap"), 5);
    glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "bEnableShadowMapping"), 1);
    DrawScene();
    glUseProgram(0);
}

void SceneManager::ComputeModelMatrix_ShadowMapping(int i)
{
    if (ShadowMap::IsDepthTesting())
    {
        ShadowMap::CalculateMVPMatrix(_gameObjects[i].second);
        glUniformMatrix4fv(glGetUniformLocation(MainEngine::shaders["Depth"], "depthMVP"), 1, GL_FALSE,
                           ShadowMap::GetDepthMVP());
    }

    if (ShadowMap::IsMapping())
    {
        ShadowMap::CalculateBiasMVPMatrix(_gameObjects[i].second);
        glUniformMatrix4fv(glGetUniformLocation(MainEngine::shaders["MainShader"], "DepthBiasMVP"), 1, GL_FALSE,
                           ShadowMap::GetDepthBiasMVP());
    }
}

void SceneManager::DrawCubeMap(VisibleGameObject * model)
{
    glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "bEnableReflection"), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, Reflection::GetFrameBuffer());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, Reflection::GetRenderBuffer());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Reflection::GetRenderBuffer());
    glPushMatrix();
    glm::vec3 lookat;
    glm::vec3 upVector;
    glm::vec3 position = model->GetPosition();

    // Render Normally
    for (GLenum i = GL_TEXTURE_CUBE_MAP_POSITIVE_X; i < GL_TEXTURE_CUBE_MAP_NEGATIVE_Z + 1; i++)
    {
        switch (i)
        {
            case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
                lookat = glm::vec3(1, 0, 0) + position;
                upVector = glm::vec3(0, -1, 0);
                break;

            case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
                lookat = glm::vec3(-1, 0, 0) + position;
                upVector = glm::vec3(0, -1, 0);
                break;

            case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
                lookat = glm::vec3(0, 1, 0) + position;
                upVector = glm::vec3(0, 0, 1);
                break;

            case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
                lookat = glm::vec3(0, -1, 0) + position;
                upVector = glm::vec3(0, 0, -1);
                break;

            case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
                lookat = glm::vec3(0, 0, 1) + position;
                upVector = glm::vec3(0, -1, 0);
                break;

            case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
                lookat = glm::vec3(0, 0, -1) + position;
                upVector = glm::vec3(0, -1, 0);
                break;

            default:
                break;
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90, 1, 1, 2000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(position.x, position.y, position.z,
                  lookat.x, lookat.y, lookat.z,
                  upVector.x, upVector.y, upVector.z);
        glViewport(0, 0, Reflection::GetSize().x, Reflection::GetSize().y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, i, Reflection::GetBox2CubeMap(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int indexModel = 0; indexModel < _gameObjects.size(); indexModel++)
        {
            if (_gameObjects[indexModel].second->GetEnableRender() && _gameObjects[indexModel].second != model)
            {
                _gameObjects[indexModel].second->Draw();
            }
        }

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    glPopMatrix();
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, MainEngine::_mainWindow->getSize().x, MainEngine::_mainWindow->getSize().y);
    glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "bEnableReflection"), 1);
    glm::mat4 invViewMatrix = glm::inverse(glm::lookAt(DefaultCamera::position, DefaultCamera::lookat, glm::vec3(0, 1, 0)));
    glUniformMatrix4fv(glGetUniformLocation(MainEngine::shaders["MainShader"], "invView"), 1, GL_FALSE,
                       &invViewMatrix[0][0]);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Reflection::GetBox2CubeMap());
    glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "CubeMap"), 6);
}
