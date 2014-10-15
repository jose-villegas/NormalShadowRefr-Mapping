#pragma once
#include "ModelVBO.h"
typedef std::map<std::string, GLuint> ModelTextures;

class VisibleGameObject
{
    public:
        ModelOBJ * GetOBJ() { return &obj; }
        VisibleGameObject(string filename);
        VisibleGameObject(void);
        bool GetEnableRender() const { return _enableRender; }
        bool GetIsLoaded() const { return _isLoaded; }
        bool GetIsReflective() const { return _isReflective; }
        bool GetIsRefractive() const { return _isRefractive; }
        float GetScaleFactor() const { return scaleFactor; }
        bool GetIsPlanar() const { return _isPlanar; }
        void SetScaleFactor(float val) { scaleFactor = val; }
        float * GetObjectRotation();
        glm::mat4x4 GetRotationMatrix() const { return _rotationMatrix; }
        glm::vec3 GetPosition() const { return _position; }
        std::string GetFilepath() const { return _filepath; }
        string GetManagerName() const { return _managerName; }
        virtual void Draw();
        virtual void Load(std::string filename);
        virtual void Update(float elapsedTime, sf::Event input);
        virtual ~VisibleGameObject(void);
        void EnableProgrammablePipeline(bool val) { _useProgrammablePipeline = val; }
        void EnableRender(bool val) { _enableRender = val; }
        void EnableTextures(bool val) { _enableTextures = val; }
        void Scale(float scale);
        void SetIsLoaded(bool val) { _isLoaded = val; }
        void SetIsReflective(bool val) { _isReflective = val; }
        void SetIsRefractive(bool val) { _isRefractive = val; }
        void SetIsPlanar(bool val) { _isPlanar = val; }
        void SetManagerName(std::string val) { _managerName = val; }
        void SetPosition(float x, float y, float z);
        void SetRotation(float pitch, float yaw, float roll);
        void SetRotation(float quat[4]);
        float GetRefractiveIndex() const { return refractiveIndex; }
        void SetRefractiveIndex(float val) { refractiveIndex = val; }
    private:
        GLuint LoadTexture(const char * pszFilename);
        ModelOBJ obj;
        ModelTextures _modelTextures;
        ModelVBO vbo;
        bool _enableRender;
        bool _enableTextures;
        bool _isLoaded;
        bool _isReflective;
        bool _isRefractive;
        bool _isPlanar;

        float refractiveIndex;

        float scaleFactor;
        bool _useProgrammablePipeline;
        glm::mat4x4 _rotationMatrix;
        glm::vec3 _position;
        string _filepath;
        string _managerName;
        void DrawFixedPipeline();
        void DrawProgrammablePipeline();
};
