#include "Light.h"
#pragma once
#include "MainEngine.h"
class ShadowMap
{
    public:
        static GLuint GetDepthMap(int i) { return _depthMap[i]; }
        static GLuint GetFrameBuffer(int i) { return _fbo[i]; }
        static bool Create(unsigned int width, unsigned int height);
        static bool IsDepthTesting() { return depthTesting; }
        static bool IsMapping() { return mapping; }
        static const GLfloat * GetDepthBiasMVP(int i) { return &depthBiasMVP[i][0][0]; }
        static const GLfloat * GetDepthMVP(int i) { return &depthMVP[i][0][0]; }
        static glm::mat4 CalculateBiasMVPMatrix(VisibleGameObject * model, int lightIndex);
        static glm::mat4 CalculateDepthVP(Light * light, int lightIndex);
        static glm::mat4 CalculateMVPMatrix(VisibleGameObject * model, int lightIndex);
        static glm::vec2 GetSize() { return size; }
        static void Delete();
        static void DepthTestingMode();
        static void MappingMode();
    private:
        static glm::vec2 size;
        static bool mapping;
        static bool depthTesting;
        static GLuint _fbo[NUM_LIGHTS];
        static GLuint _depthMap[NUM_LIGHTS];
        // This matrix stays the same between models
        static glm::mat4 depthVP[NUM_LIGHTS];
        // This matrix changes between models
        static glm::mat4 depthMVP[NUM_LIGHTS];
        static glm::mat4 depthBiasMVP[NUM_LIGHTS];
        // Bias Matrix
        static glm::mat4 biasMatrix;
};

