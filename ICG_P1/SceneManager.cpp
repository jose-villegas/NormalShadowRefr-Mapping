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

// This matrix stays the same between models
glm::mat4 depthVP;
bool depthTesting = false;
bool mapping = false;
// This matrix changes between models
glm::mat4 depthMVP;
glm::mat4 depthBiasMVP;
// Bias Matrix
glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

glm::mat4 CalculateDepthVP(Light * light)
{
    float aspectRatio = MainEngine::_mainWindow->getSize().x / MainEngine::_mainWindow->getSize().y;
    // Light Params
    glm::vec3 lightInvDir = glm::vec3(-light->direction[0], -light->direction[1], -light->direction[2]);
    glm::vec3 lightPos = glm::vec3(light->position[0], light->position[1], light->position[2]);
    // Matrices
    //glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos - lightInvDir, glm::vec3(0, 1, 0));
    //glm::mat4 depthProjectionMatrix = glm::perspective<float>(90.0f, aspectRatio, 1.0f, 2000.0f);
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-300, 300, -300, 300, 1, 2000);
    glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 depthModelMatrix = glm::mat4(1.0);
    // MVP
    return depthProjectionMatrix * depthViewMatrix;
}

void SceneManager::Draw()
{
    if (MainEngine::_enableShadows)
    {
        // Render from Light's POV
        depthTesting = true;
        glBindFramebuffer(GL_FRAMEBUFFER, MainEngine::m_shadowMapFBO.GetFbo());
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glUseProgram(MainEngine::shaders["Depth"]);
        depthVP = CalculateDepthVP(MainEngine::light[0]);
        DrawUsingFixedPipeline();
        // Render from Camera's POV
        depthTesting = false;
        mapping = true;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
        glUseProgram(MainEngine::shaders["MainShader"]);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, MainEngine::m_shadowMapFBO.GetShadowMap());
        glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "shadowMap"), 5);
        glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "bEnableShadowMapping"), 1);
        DrawScene();
        mapping = false;
    }
    else
    {
        glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "bEnableShadowMapping"), 0);
        DrawScene();
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
    if (MainEngine::_enableBumpMapping) { glUseProgram(MainEngine::shaders["MainShader"]); }

    for (int i = 0; i < _gameObjects.size(); i++)
    {
        if (_gameObjects[i].second->ToBeDeleted())
        {
            RemoveAt(i);
        }
        else
        {
            if (depthTesting)
            {
                glm::mat4 depthModel = glm::mat4(1);
                depthModel = glm::translate(depthModel, _gameObjects[i].second->GetPosition());
                depthModel *= _gameObjects[i].second->GetRotationMatrix();
                depthMVP = depthVP * depthModel;
                glUniformMatrix4fv(glGetUniformLocation(MainEngine::shaders["Depth"], "depthMVP"), 1, GL_FALSE, &depthMVP[0][0]);
            }

            if (mapping)
            {
                glm::mat4 depthModel = glm::mat4(1);
                depthModel = glm::translate(depthModel, _gameObjects[i].second->GetPosition());
                depthModel *= _gameObjects[i].second->GetRotationMatrix();
                depthMVP = depthVP * depthModel;
                depthBiasMVP = biasMatrix * depthMVP;
                glUniformMatrix4fv(glGetUniformLocation(MainEngine::shaders["MainShader"], "DepthBiasMVP"), 1, GL_FALSE,
                                   &depthBiasMVP[0][0]);
            }

            _gameObjects[i].second->Draw();
        }
    }

    glUseProgram(0);
}

void SceneManager::DrawUsingFixedPipeline()
{
    for (int i = 0; i < _gameObjects.size(); i++)
    {
        if (_gameObjects[i].second->ToBeDeleted())
        {
            RemoveAt(i);
        }
        else
        {
            if (depthTesting)
            {
                glm::mat4 depthModel = glm::mat4(1);
                depthModel = glm::translate(depthModel, _gameObjects[i].second->GetPosition());
                depthModel *= _gameObjects[i].second->GetRotationMatrix();
                depthMVP = depthVP * depthModel;
                glUniformMatrix4fv(glGetUniformLocation(MainEngine::shaders["Depth"], "depthMVP"), 1, GL_FALSE, &depthMVP[0][0]);
            }

            _gameObjects[i].second->EnableProgrammablePipeline(false);

            if (depthTesting) { _gameObjects[i].second->EnableTextures(false); }

            _gameObjects[i].second->Draw();

            if (depthTesting) { _gameObjects[i].second->EnableProgrammablePipeline(true); }

            _gameObjects[i].second->EnableTextures(true);
        }
    }

    glUseProgram(0);
}