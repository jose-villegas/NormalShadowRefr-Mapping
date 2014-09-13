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

void SetTextureMatrix(void)
{
	static double modelView[16];
	static double projection[16];
	// This is matrix transform every coordinate x,y,z
	// x = x* 0.5 + 0.5
	// y = y* 0.5 + 0.5
	// z = z* 0.5 + 0.5
	// Moving from unit cube [-1,1] to [0,1]
	const GLdouble bias[16] =
	{
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	};
	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glLoadIdentity();
	glLoadMatrixd(bias);
	// concatating all matrice into one.
	glMultMatrixd(projection);
	glMultMatrixd(modelView);
	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void SetupMatrices(float position_x, float position_y, float position_z, float lookAt_x, float lookAt_y, float lookAt_z)
{
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(90, MainEngine::_mainWindow->getSize().x / MainEngine::_mainWindow->getSize().y, 1.f, 40000);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	gluLookAt(position_x, position_y, position_z, lookAt_x, lookAt_y, lookAt_z, 0, 1, 0);
}

void SceneManager::Draw()
{
	// ---------------------------------------------------------------------------------------------------------------------------------------
	// ---- Draw from Light POV
	glBindFramebuffer(GL_FRAMEBUFFER, MainEngine::m_shadowMapFBO.GetFbo());
	glViewport(0, 0, MainEngine::_mainWindow->getSize().x, MainEngine::_mainWindow->getSize().y);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, MainEngine::_mainWindow->getSize().x / MainEngine::_mainWindow->getSize().y, 1.f, 40000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	DefaultCamera::SetPosition(sf::Vector3f(MainEngine::light[0]->position[0],
											MainEngine::light[0]->position[1],
											MainEngine::light[0]->position[2]));
	DefaultCamera::LookAt(sf::Vector3f(0, 0, -100));
	DefaultCamera::UpdateCamera();
	glCullFace(GL_FRONT);
	SetTextureMatrix();
	MainEngine::_enableShader = false;
	DrawUsingFixedPipeline();
	// ---------------------------------------------------------------------------------------------------------------------------------------
	// ---- Draw from Camera POV
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, MainEngine::_mainWindow->getSize().x, MainEngine::_mainWindow->getSize().y);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(MainEngine::shaders["Shadow-Mapping"]);
	glUniform1i(glGetUniformLocation(MainEngine::shaders["Shadow-Mapping"], "ShadowMap"), 7);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, MainEngine::m_shadowMapFBO.GetShadowMap());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, MainEngine::_mainWindow->getSize().x / MainEngine::_mainWindow->getSize().y, 1.f, 40000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	DefaultCamera::SetPosition(sf::Vector3f(0, 0, 0));
	DefaultCamera::LookAt(sf::Vector3f(0, 0, -100));
	DefaultCamera::UpdateCamera();
	glCullFace(GL_BACK);
	MainEngine::_enableShader = false;
	DrawUsingFixedPipeline();
	// ---------------------------------------------------------------------------------------------------------------------------------------
	// ---- Debug Show Depth Texture
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-int(MainEngine::_mainWindow->getSize().x) / 2, MainEngine::_mainWindow->getSize().x / 2,
			-int(MainEngine::_mainWindow->getSize().y) / 2, MainEngine::_mainWindow->getSize().y / 2, 1, 650);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4f(1, 1, 1, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, MainEngine::m_shadowMapFBO.GetShadowMap());
	glEnable(GL_TEXTURE_2D);
	glTranslated(0, 0, -1);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3f(0, 0, 0);
	glTexCoord2d(1, 0); glVertex3f(MainEngine::_mainWindow->getSize().x / 2, 0, 0);
	glTexCoord2d(1, 1); glVertex3f(MainEngine::_mainWindow->getSize().x / 2, MainEngine::_mainWindow->getSize().y / 2, 0);
	glTexCoord2d(0, 1); glVertex3f(0, MainEngine::_mainWindow->getSize().y / 2, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	// -------------------------------------------------------------------------------------------------------------------------------------
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
