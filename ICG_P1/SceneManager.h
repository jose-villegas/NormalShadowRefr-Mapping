#pragma once
#include "VisibleGameObject.h"
#include "Light.h"

class SceneManager
{
    public:
        SceneManager(void);
        ~SceneManager(void);

        void Add(std::string name, VisibleGameObject * gameObject);
        void Remove(std::string name);
        void RemoveAt(int index);
        int GetObjectCount() const;
        VisibleGameObject * Get(string name) const;
        VisibleGameObject * GetAt(int index) const;
        void Draw();
        void DrawUsingFixedPipeline();
        void UpdateAll(sf::Event input);
        void CleanEnemies();
        vector<pair <std::string, VisibleGameObject *>> GetGameObjects() const { return _gameObjects; }
    private:
        vector<pair <std::string, VisibleGameObject *>> _gameObjects;

        struct GameObjectDeallocator
        {
            void operator()(const std::pair<std::string, VisibleGameObject *> &p) const
            {
                delete p.second;
            }
        };

        struct CompareFirst
        {
                CompareFirst(string val) : val_(val) {}
                bool operator()(const pair <std::string, VisibleGameObject *> &elem) const
                {
                    return val_ == elem.first;
                }
            private:
                string val_;
        };

        void DrawScene();
};
