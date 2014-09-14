#pragma once
#include "ModelVBO.h"
typedef std::map<std::string, GLuint> ModelTextures;

#define FIRST_PLAYER	1000
#define ENEMY			1001
#define PROJECTILE		1002
#define DEBRIS			1003
#define WALL_OBJECT		1004
#define DEFAULT			1005

class VisibleGameObject
{
    public:
        VisibleGameObject(void);
        VisibleGameObject(string filename);
        virtual ~VisibleGameObject(void);
        virtual void Load(std::string filename);
        virtual void Draw();
        virtual void Update(float elapsedTime, sf::Event input);
        virtual void SetPosition(float x, float y, float z);
        virtual void RotateObject(float pitch, float yaw, float roll);
        virtual void RotateObject(float quat[4]);
        virtual float * GetObjectRotation();
        virtual void Scale(float scale);
        virtual void EnableTextures(bool val) { _enableTextures = val; }
        virtual void EnableProgrammablePipeline(bool val) { _useProgrammablePipeline = val; }
        virtual void EnableRender(bool val) { _enableRender = val; }
        virtual bool IsLoaded() const;
        virtual void SetToBeDeleted(bool val) { _toBeDeleted = val; }
        virtual void SetManagerName(std::string val) { _managerName = val; }
        virtual bool ToBeDeleted() const { return _toBeDeleted; }
        virtual int GetTypeTag() const { return _typeTag; }
        virtual void SetTypeTag(int val) { _typeTag = val; }
        virtual glm::vec3 GetPosition() const { return _position; }
        virtual glm::mat4x4 GetRotationMatrix() const { return _rotationMatrix; }
        virtual std::string GetFilepath() const { return _filepath; }
        virtual string GetManagerName() const { return _managerName; }
        virtual ModelOBJ * GetOBJ() { return &obj; }

    private:
        ModelOBJ obj;
        ModelVBO vbo;
        ModelTextures _modelTextures;
        string _filepath;
        string _managerName;
        glm::vec3 _position;
        glm::mat4x4 _rotationMatrix;
        bool _isLoaded;
        bool _enableTextures;
        bool _useProgrammablePipeline;
        bool _enableRender;
        bool _toBeDeleted;
        int _typeTag;
        GLuint LoadTexture(const char * pszFilename);
        void DrawFixedPipeline();
        void DrawProgrammablePipeline();

    public:
        float _roughness;
        GLuint vertexBuffer, uvBuffer, normalBuffer, tangentBuffer, bitangentBuffer, indexBuffer;
};
