#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Transformation.hh>
#include <raindance/Core/Camera/Camera.hh>

class TimeSeries : public Document::Node
{
public:
	TimeSeries(Document::Node* parent = NULL)
	: Document::Node(parent)
	{       
		// background().loadImage("TimeSeries/miller", FS::BinaryFile("Assets/Earth/miller-bw-cropped.png"));

		
	}

	void idle(Context* context) override
	{
		(void) context;
	}
};