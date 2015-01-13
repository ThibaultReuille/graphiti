#pragma once

class WorldResources
{
public:
	WorldResources()
	{
		EarthGeoPointShader = ResourceManager::getInstance().loadShader("cube", Assets_cube_vert, sizeof(Assets_cube_vert), Assets_cube_frag, sizeof(Assets_cube_frag));
		EarthGeoCube = new Cube(glm::vec3(0.0, 0.5, 0.0));

		StarfieldTexture = ResourceManager::getInstance().loadTexture("starfield", Assets_Spheres_starfield_png, sizeof(Assets_Spheres_starfield_png));
		Starfield = new EnvironmentSphere();
		Starfield->setTexture(StarfieldTexture);

		WorldMapNodeIcon = new Icon();
		WorldMapNodeIcon->load("ball", Assets_Particle_ball_png, sizeof(Assets_Particle_ball_png));
		WorldMapNodeIconSize = 10.0;

		EarthRadius = 20.0f;
		EarthNodeSize = 0.25;
		EarthGeoLinkShader = ResourceManager::getInstance().loadShader("curve", Assets_curve_vert, sizeof(Assets_curve_vert), Assets_curve_frag, sizeof(Assets_curve_frag));
	}

	~WorldResources()
	{
	    delete WorldMapNodeIcon;
		delete Starfield;
		ResourceManager::getInstance().unload(StarfieldTexture);

		delete EarthGeoCube;
		ResourceManager::getInstance().unload(EarthGeoPointShader);
		ResourceManager::getInstance().unload(EarthGeoLinkShader);
	}

	GraphModel* Model;

	Light Sun;
	Material EarthGeoPointMaterial;
	Shader::Program* EarthGeoPointShader;
    Shader::Program* EarthGeoLinkShader;
    float EarthRadius;
	Cube* EarthGeoCube;
    float EarthNodeSize;

	Texture* StarfieldTexture;
	EnvironmentSphere* Starfield;

	Icon* WorldMapNodeIcon;
	float WorldMapNodeIconSize;

};

WorldResources* g_WorldResources = NULL;

