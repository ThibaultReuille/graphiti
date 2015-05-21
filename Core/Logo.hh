#pragma once

#include <raindance/Core/Headers.hh>
#include <raindance/Core/Transformation.hh>
#include <raindance/Core/Camera/Camera.hh>
#include <raindance/Core/Icon.hh>

class Logo : public Document::Node
{
public:
	Logo(Document::Node* parent = NULL)
	: Document::Node(parent)
	{       
		background().loadImage("umbrella-logo.png", FS::BinaryFile("Assets/umbrella-logo.png")); 
	}
};