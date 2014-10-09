#pragma once

class WorldResources
{
public:
	WorldResources()
	{
		EarthGeoPointShader = ResourceManager::getInstance().loadShader("cube", Resources_cube_vert, sizeof(Resources_cube_vert), Resources_cube_frag, sizeof(Resources_cube_frag));
		EarthGeoCube = new Cube(glm::vec3(0.0, 0.5, 0.0));

		StarfieldTexture = ResourceManager::getInstance().loadTexture("starfield", Resources_Spheres_starfield_png, sizeof(Resources_Spheres_starfield_png));
		Starfield = new EnvironmentSphere();
		Starfield->setTexture(StarfieldTexture);

		WorldMapNodeIcon = new Icon();
		WorldMapNodeIcon->load("ball", Resources_Particle_ball_png, sizeof(Resources_Particle_ball_png));
		WorldMapNodeIconSize = 10.0;

		EarthRadius = 20.0f;
		EarthNodeSize = 0.25;
		EarthGeoLinkShader = ResourceManager::getInstance().loadShader("curve", Resources_curve_vert, sizeof(Resources_curve_vert), Resources_curve_frag, sizeof(Resources_curve_frag));
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

