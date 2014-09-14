#pragma once
class DefaultCamera
{
    public:
        static sf::Vector3f position;
        static sf::Vector3f lookat;
        static sf::Vector3f direction;
        static sf::Vector3f upVector;
        static void UpdateCamera();
        static const sf::Vector3f GetPosition() { return position; }
        static void LookAt(sf::Vector3f model);
        static void SetPosition(sf::Vector3f pos);
};
