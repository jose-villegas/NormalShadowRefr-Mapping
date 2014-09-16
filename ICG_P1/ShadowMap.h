#include "Light.h"
#pragma once
class ShadowMap
{
    public:
        static GLuint GetDepthMap() { return _depthMap; }
        static GLuint GetFrameBuffer() { return _fbo; }
        static bool Create(unsigned int width, unsigned int height);
        static bool IsDepthTesting() { return depthTesting; }
        static bool IsMapping() { return mapping; }
        static const GLfloat * GetDepthBiasMVP() { return &depthBiasMVP[0][0]; }
        static const GLfloat * GetDepthMVP() { return &depthMVP[0][0]; }
        static glm::mat4 CalculateBiasMVPMatrix(VisibleGameObject * model);
        static glm::mat4 CalculateDepthVP(Light * light);
        static glm::mat4 CalculateMVPMatrix(VisibleGameObject * model);
        static glm::vec2 GetSize() { return size; }
        static void Delete();
        static void DepthTestingMode();
        static void MappingMode();
    private:
        static glm::vec2 size;
        static bool mapping;
        static bool depthTesting;
        static GLuint _fbo;
        static GLuint _depthMap;
        // This matrix stays the same between models
        static glm::mat4 depthVP;
        // This matrix changes between models
        static glm::mat4 depthMVP;
        static glm::mat4 depthBiasMVP;
        // Bias Matrix
        static glm::mat4 biasMatrix;
};

