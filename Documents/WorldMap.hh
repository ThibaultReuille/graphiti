#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Transformation.hh>
#include <raindance/Core/Camera/Camera.hh>

class WorldMap : public Document::Node
{
public:
	WorldMap(Document::Node* parent = NULL)
	: Document::Node(parent)
	{       
		background().loadImage("WorldMap/miller", FS::BinaryFile("Assets/Earth/miller-bw-cropped.png"));
	}

	void idle(Context* context) override
	{
		(void) context;
	}
};