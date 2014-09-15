#pragma once
class DefaultCamera
{
    public:
        static glm::vec3 direction;
        static glm::vec3 lookat;
        static glm::vec3 position;
        static glm::vec3 upVector;
        static void LookAt(glm::vec3 model);
        static void SetPosition(glm::vec3 pos);
        static void UpdateCamera();
};
