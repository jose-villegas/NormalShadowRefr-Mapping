#pragma once
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

class GraphicSettings
{
    public:
        GraphicSettings(void);
        ~GraphicSettings(void);
        static void setAnistrophy(float value);
        static float getAnisotrophyValue() { return anisotrophyValue; }
        static float getRenderHeight() { return renderHeight; }
        static float getRenderWidth() { return renderWidth; }
        static sf::ContextSettings getSettings() { return settings; }
        static void setRenderHeight(float val) { renderHeight = val; }
        static void setRenderWidth(float val) { renderWidth = val; }
        static void setSettings(sf::ContextSettings val) { settings = val; }
        static float getDistanceDebris() { return distanceDebris; }
        static void setDistanceDebris(float val) { distanceDebris = val; }
        static float getLifetimeProjectiles() { return lifetimeProjectiles; }
        static void setLifetimeProjectiles(float val) { lifetimeProjectiles = val; }
		static void setWidthHeight(unsigned short w, unsigned short h);
    private:
        static float anisotrophyValue;
        static float renderWidth;
        static float renderHeight;
        static float distanceDebris;
        static float lifetimeProjectiles;
        static sf::ContextSettings settings;
};

