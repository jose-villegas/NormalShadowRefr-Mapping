#pragma once
#include "VisibleGameObject.h"
class Reflection
{
    public:
        static GLuint GetBox2CubeMap() { return box2CubeMap; }
        static GLuint GetRenderBuffer() { return box2depthRender; }
        static GLuint GetDepthTexture() { return depthTexture; }
        static GLuint GetFrameBuffer() { return _fbo; }
        static glm::vec2 GetSize() { return size; }
        static bool Create(unsigned int width, unsigned int height);
        static void Delete();
    private:
        static glm::vec2 size;

        static GLuint _fbo;
        static GLuint box2CubeMap;
        static GLuint box2depthRender;
        static GLuint depthTexture;
};

