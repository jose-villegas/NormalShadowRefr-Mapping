#include "stdafx.h"
#include "SceneManager.h"
#include "GraphicSettings.h"
#include "MainEngine.h"
#include "DefaultCamera.h"
#include "glm/gtc/matrix_transform.hpp"
#include <SDL_stdinc.h>

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

glm::mat4 CalculateDepthMVP(Light * light)
{
    float aspectRatio = MainEngine::_mainWindow->getSize().x / MainEngine::_mainWindow->getSize().y;
    glm::vec3 lightInvDir = glm::vec3(-light->direction[0], -light->direction[1], -light->direction[2]);
    glm::vec3 lightPos = glm::vec3(light->position[0], light->position[1], light->position[2]);
    // Matrices
    glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos - lightInvDir, glm::vec3(0, 1, 0));
    glm::mat4 depthProjectionMatrix = glm::perspective<float>(90.0f, aspectRatio, 1.0f, 2000.0f);
    glm::mat4 depthModelMatrix = glm::mat4(1.0);
    // MVP
    return depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
}

void SceneManager::Draw()
{
    MainEngine::_enableShader = false;
    glBindFramebuffer(GL_FRAMEBUFFER, MainEngine::m_shadowMapFBO.GetFbo());
    glClear(GL_DEPTH_BUFFER_BIT);
    glUniformMatrix4fv(MainEngine::shaders["Depth"], 1, GL_FALSE, &CalculateDepthMVP(MainEngine::light[0])[0][0]);
    glUseProgram(MainEngine::shaders["Depth"]);
    DrawUsingFixedPipeline();
    // -
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glUseProgram(MainEngine::shaders["Shadow-Mapping"]);
    //glUniform1i(glGetUniformLocation(MainEngine::shaders["Shadow-Mapping"], "ShadowMap"), 7);
    //glActiveTexture(GL_TEXTURE7);
    //glBindTexture(GL_TEXTURE_2D, MainEngine::m_shadowMapFBO.GetShadowMap());
    //RestoreCameraToOriginCoords();
    DrawUsingFixedPipeline();
    // ---- Debug Show Depth Texture
    glPushMatrix();
    glOrtho(-int(MainEngine::_mainWindow->getSize().x) / 2, MainEngine::_mainWindow->getSize().x / 2,
            -int(MainEngine::_mainWindow->getSize().y) / 2, MainEngine::_mainWindow->getSize().y / 2, 1, 2000);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, MainEngine::m_shadowMapFBO.GetShadowMap());
    glEnable(GL_TEXTURE_2D);
    glTranslated(0, 0, - 1);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2d(1, 0); glVertex3f(MainEngine::_mainWindow->getSize().x / 2, 0, 0);
    glTexCoord2d(1, 1); glVertex3f(MainEngine::_mainWindow->getSize().x / 2, MainEngine::_mainWindow->getSize().y / 2, 0);
    glTexCoord2d(0, 1); glVertex3f(0, MainEngine::_mainWindow->getSize().y / 2, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void SceneManager::UpdateAll(sf::Event input)
{
    for (int i = 0; i < _gameObjects.size(); i++)
    {
        if (_gameObjects[i].second->ToBeDeleted())
        {
            RemoveAt(i);
        }
        else
        {
            _gameObjects[i].second->Update(MainEngine::GetFrameTime(), input);
        }
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

void SceneManager::CleanEnemies()
{
    for (int i = 0; i < _gameObjects.size(); i++)
    {
        if (_gameObjects[i].second->GetTypeTag() == ENEMY)
        {
            _gameObjects[i].second->EnableRender(false);
            RemoveAt(i);
        }
    }
}

void SceneManager::DrawScene()
{
    if (MainEngine::_enableShader) { glUseProgram(MainEngine::shaders["MainShader"]); }

    for (int i = 0; i < _gameObjects.size(); i++)
    {
        if (_gameObjects[i].second->ToBeDeleted())
        {
            RemoveAt(i);
        }
        else
        {
            _gameObjects[i].second->Draw();
        }
    }

    glUseProgram(0);
}

void SceneManager::DrawUsingFixedPipeline()
{
    if (MainEngine::_enableShader) { glUseProgram(MainEngine::shaders["MainShader"]); }

    for (int i = 0; i < _gameObjects.size(); i++)
    {
        if (_gameObjects[i].second->ToBeDeleted())
        {
            RemoveAt(i);
        }
        else
        {
            _gameObjects[i].second->EnableProgrammablePipeline(false);
            _gameObjects[i].second->Draw();
            _gameObjects[i].second->EnableProgrammablePipeline(true);
        }
    }

    glUseProgram(0);
}
