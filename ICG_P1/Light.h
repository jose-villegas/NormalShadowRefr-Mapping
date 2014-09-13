#pragma once
#include "VisibleGameObject.h"

class Light
{
    public:

        float position[4];
        float direction[4];
        float ambient[4];
		float diffuse[4];
		float specular[4];
        float intensity;
		float const_att;
		float linear_att;
		float quad_att;
		float cutoff;
		float spot_exp;
		bool active;
        int lightType;

    public:

        Light(void);
        ~Light(void);
        void SetPosition(float pos[4]);
        void SetPosition(float x, float y, float z);
        void SetDiffuse(float diff[4]);
        void SetDiffuse(float x, float y, float z, float w);
        void Enable(GLenum lightN);
        void Disable(GLenum lightN);
        void Draw(GLenum lightN);
        void Update(float elapsedTime, sf::Event input);

};

